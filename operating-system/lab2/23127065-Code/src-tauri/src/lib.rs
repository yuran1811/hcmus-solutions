#[cfg_attr(mobile, tauri::mobile_entry_point)]
use tauri::{
    menu::{Menu, MenuItem},
    tray::TrayIconBuilder,
};

mod fs_operations;
mod partitions;

use fs_operations::{get_file_metadata, read_immediate_children, DirectoryNode, FileMetadata};
use partitions::{detect_partitions, PartitionInfo};

#[tauri::command]
fn list_partitions() -> Vec<PartitionInfo> {
    detect_partitions()
}

#[tauri::command]
fn get_children(path: String) -> Result<DirectoryNode, String> {
    read_immediate_children(&path)
}

#[tauri::command]
fn get_file_details(path: String) -> Result<FileMetadata, String> {
    get_file_metadata(&path)
}

#[tauri::command]
fn read_text_file(path: String) -> Result<String, String> {
    fs_operations::read_text_file(&path)
}

pub fn run() {
    tauri::Builder::default()
        .setup(|app| {
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
            read_text_file
        ])
        .plugin(tauri_plugin_opener::init())
        .plugin(tauri_plugin_shell::init())
        .plugin(tauri_plugin_notification::init())
        .plugin(tauri_plugin_os::init())
        .plugin(tauri_plugin_fs::init())
        .plugin(tauri_plugin_store::Builder::new().build())
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
