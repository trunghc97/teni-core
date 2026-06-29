# OTA guide (future)

Network OTA is not implemented and MVP1.1 remains fully offline. The large factory partition stores
firmware and linked assets. A small `assetpack` data partition is reserved for a future signed update
transport over local USB; it is not written by current firmware.

A production implementation should add an `IFirmwareUpdater` service, never couple update transport
to the application, verify signed manifests and image hashes, require adequate battery/external
power, write only to the inactive slot, confirm health after reboot, and roll back automatically.
TLS trust anchors and anti-rollback version counters belong in protected storage. Update progress and
failure should be events so product-specific LED/voice behavior remains outside the updater.
