use std::{sync::Arc, time::Duration};

use models::fat_fs::FAT32FileSystem;
use serde_json::json;
#[cfg_attr(mobile, tauri::mobile_entry_point)]
use tauri::{
    menu::{Menu, MenuItem},
    tray::TrayIconBuilder,
    Wry,
};
use tauri_plugin_store::{Store, StoreExt};

pub mod fs_operations;
pub mod models;
pub mod partitions;
pub mod utils;

use crate::{
    fs_operations::get_file_metadata,
    models::{
        fat_fs::FAT32,
        fs::{DirectoryNode, FileMetadata},
        partition::PartitionInfo,
    },
    utils::list_partitions::list_fat32_paritions_by_letter,
};

static mut GLOBAL_STORE: Option<Arc<Store<Wry>>> = None;

#[allow(static_mut_refs)]
fn get_store() -> Option<Arc<Store<Wry>>> {
    unsafe { GLOBAL_STORE.clone() }
}

fn get_store_value(key: &str) -> Option<serde_json::Value> {
    let store = get_store();
    if store.is_none() {
        return None;
    }

    let store = store.unwrap();
    store.get(key)
}

#[tauri::command]
fn update_working_drive(drive: &str) {
    let store = get_store();
    if store.is_none() {
        return;
    }

    let store = store.unwrap();
    store.set(
        "working_drive",
        json!({
            "label": drive,
            "last_updated": chrono::Utc::now().timestamp_millis()
        }),
    );
}

#[tauri::command]
fn list_partitions() -> Vec<PartitionInfo> {
    list_fat32_paritions_by_letter()
    // detect_partitions()
}

#[tauri::command]
fn get_children(path: String) -> Result<DirectoryNode, String> {
    let cur_drive = get_store_value("working_drive");
    if cur_drive.is_none() {
        return Err("No current drive found in store".to_string());
    }

    let cur_drive = cur_drive.unwrap();
    let part = format!("\\\\.\\{}", cur_drive["label"].as_str().unwrap());
    match FAT32::open(&part, None) {
        Ok(mut f) => match f.read_immediate_children(&path) {
            Ok(children) => Ok(children),
            Err(_) => Err("Error reading children".to_string()),
        },
        Err(_) => Err("Error opening FAT32".to_string()),
    }

    // read_immediate_children(&path)
}

#[tauri::command]
fn get_file_details(path: String) -> Result<FileMetadata, String> {
    get_file_metadata(&path)
}

#[tauri::command]
fn read_text_file(path: String) -> Result<String, String> {
    let cur_drive = get_store_value("working_drive");
    if cur_drive.is_none() {
        return Err("No current drive found in store".to_string());
    }

    let cur_drive = cur_drive.unwrap();
    let part = format!("\\\\.\\{}", cur_drive["label"].as_str().unwrap());
    match FAT32::open(&part, None) {
        Ok(mut f) => match f.read_file(&path, Some(true)) {
            Ok((_, content)) => Ok(content),
            Err(_) => Err("Error reading file".to_string()),
        },
        Err(_) => Err("Error opening FAT32".to_string()),
    }

    // fs_operations::read_text_file(&path)
}

pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_opener::init())
        .plugin(tauri_plugin_shell::init())
        .plugin(tauri_plugin_notification::init())
        .plugin(tauri_plugin_os::init())
        .plugin(tauri_plugin_fs::init())
        .plugin(tauri_plugin_store::Builder::default().build())
        .setup(|app| {
            let store = app
                .store_builder("global_store.json")
                .auto_save(Duration::from_secs(1))
                .build()
                .expect("Failed to build store");
            unsafe {
                GLOBAL_STORE = Some(store);
            }

            // Create the tray icon
            let quit_i = MenuItem::with_id(app, "quit", "Quit", true, None::<&str>)?;
            let menu = Menu::with_items(app, &[&quit_i])?;

            let _tray = TrayIconBuilder::new()
                .menu(&menu)
                .show_menu_on_left_click(true)
                .icon(app.default_window_icon().unwrap().clone())
                .on_menu_event(|app, event| match event.id.as_ref() {
                    "quit" => {
                        app.exit(0);
                    }
                    other => {
                        println!("menu item {} not handled", other);
                    }
                })
                .build(app)?;

            Ok(())
        })
        .invoke_handler(tauri::generate_handler![
            list_partitions,
            get_children,
            get_file_details,
            read_text_file,
            update_working_drive
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
