# Vietnamese story assets

The four original MVP1.1 recordings are child-friendly retellings of Vietnamese folk legends,
written for teni-core and synthesized locally with the macOS Vietnamese voice `Linh`. They remain as
cached build outputs until their text changes or a forced regeneration is requested. New or changed
text is synthesized automatically with F5-TTS Base and the Vietnamese ViVoice checkpoint described in
[TTS_PIPELINE.md](TTS_PIPELINE.md). No commercial audiobook recording is included.

| Asset | Script | Reference context |
|---|---|---|
| `cay_khe.mp3` | `content/stories/cay_khe.txt` | Vietnamese oral folk tradition |
| `chu_cuoi_cung_trang.mp3` | `content/stories/chu_cuoi_cung_trang.txt` | Vietnamese Mid-Autumn folk tradition |
| `su_tich_ho_guom.mp3` | `content/stories/su_tich_ho_guom.txt` | Hồ Hoàn Gươm legend and historical tradition |
| `banh_chung_banh_day.mp3` | `content/stories/banh_chung_banh_day.txt` | Hùng Kings folk tradition |

Audio encoding: mono MP3, 22.05 kHz, 48 kbit/s. The scripts are retained to make corrections,
re-recording, translation, and automatic voice replacement reproducible.
