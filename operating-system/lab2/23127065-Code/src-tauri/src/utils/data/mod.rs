use chrono::NaiveDate;
use std::time::{Duration, SystemTime};

pub mod parser;
pub mod reader;

pub fn convert_fat_time(date: u16, time: u32, with_ms: bool) -> SystemTime {
    let year = 1980 + (date >> 9 & 0x7F) as i32;
    let month = (date >> 5 & 0x0F) as u8;
    let day = (date & 0x1F) as u8;

    let hour = if with_ms {
        (time >> 19 & 0x1F) as u8
    } else {
        (time >> 11 & 0x1F) as u8
    };
    let minute = if with_ms {
        (time >> 13 & 0x3F) as u8
    } else {
        (time >> 5 & 0x3F) as u8
    };
    let second = if with_ms {
        (time >> 7 & 0x3F) as u8
    } else {
        (time & 0x1F) as u8 * 2
    };

    let duration = SystemTime::UNIX_EPOCH
        + Duration::from_secs(
            NaiveDate::from_ymd_opt(year, month as u32, day as u32)
                .and_then(|d| d.and_hms_opt(hour as u32, minute as u32, second as u32))
                .map(|dt| dt.and_utc().timestamp())
                .unwrap_or(0) as u64,
        );

    duration
}
