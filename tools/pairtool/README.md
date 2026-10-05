# pairtool — click-free BLE pairing for esptest-BLE-KB

Pairs the firmware's BLE HID keyboard to Windows with **no "Pair" click**, using the
WinRT `DeviceInformationCustomPairing` ConfirmOnly ceremony. PowerShell's scriptblock
`PairingRequested` handler returns `RejectedByHandler` in Windows PowerShell 5.1, so this
is a small compiled C# helper whose `PairingRequested += (s,e) => e.Accept()` fires reliably.

## Use

```
dotnet run -c Release -- esptest
```

- Argument is a substring matched against the advertising device name (default `esptest`).
- Discovers the unpaired BLE device, unpairs any stale bond, then pairs ConfirmOnly.
- Prints `PAIR-STATUS: Paired` and exits 0 on success.

Requires the .NET SDK (built/tested with SDK 10; target framework `net8.0-windows10.0.19041.0`).

## Notes

- After the first bond, Windows auto-reconnects on every boot; the firmware stores the
  responder encryption key so the bond survives reboots. Re-run this only after an
  `erase_flash` or if you remove the device in Windows Bluetooth settings.
- For fully zero-touch first-pair, register this as a logon scheduled task.
- The firmware keeps BLE advertising gentle (low TX power, ~200-250 ms, stop-on-connect)
  so it does not interfere with other nearby BLE peripherals.
