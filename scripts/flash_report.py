"""PlatformIO post-build flash usage report."""

import json
from pathlib import Path

Import("env")


def report(target, source, env):
    project = Path(env.subst("$PROJECT_DIR"))
    binary = Path(str(target[0]))
    index_path = project / "build/asset_index.json"
    index = json.loads(index_path.read_text()) if index_path.exists() else {}
    report_data = {
        "firmwareBinaryBytes": binary.stat().st_size if binary.exists() else 0,
        "embeddedAssetBytes": index.get("embeddedBytes", 0),
        "applicationPartitionBytes": int("0xEE0000", 16),
    }
    report_data["remainingApplicationBytes"] = (
        report_data["applicationPartitionBytes"] - report_data["firmwareBinaryBytes"])
    output = project / "build/flash_report.json"
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report_data, indent=2) + "\n")
    print("Flash report:", json.dumps(report_data))


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", report)
