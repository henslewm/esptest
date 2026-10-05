using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Windows.Devices.Bluetooth;
using Windows.Devices.Enumeration;

class Program
{
    static async Task<int> Main(string[] args)
    {
        string match = args.Length > 0 ? args[0] : "esptest";

        // Discover BLE devices in BOTH pairing states and find ours by name. Searching only
        // unpaired devices would miss a stale bond (e.g. after erase_flash, Windows keeps the
        // bond while the board loses its keys), leaving the unpair/re-pair path unreachable.
        var found = new List<DeviceInformation>();
        foreach (bool paired in new[] { false, true })
        {
            string sel = BluetoothLEDevice.GetDeviceSelectorFromPairingState(paired);
            foreach (var d in await DeviceInformation.FindAllAsync(sel))
            {
                found.Add(d);
            }
        }
        var di = found.FirstOrDefault(d => d.Name != null && d.Name.Contains(match));
        if (di == null)
        {
            Console.WriteLine($"NOTFOUND: no BLE device matching '{match}' (saw {found.Count}).");
            return 2;
        }
        Console.WriteLine($"FOUND: name=[{di.Name}] canPair={di.Pairing.CanPair} isPaired={di.Pairing.IsPaired} id={di.Id}");

        if (di.Pairing.IsPaired)
        {
            var u = await di.Pairing.UnpairAsync();
            Console.WriteLine($"UNPAIR: {u.Status}");
            await Task.Delay(1500);
            di = await DeviceInformation.CreateFromIdAsync(di.Id);
        }

        var custom = di.Pairing.Custom;
        custom.PairingRequested += (s, e) =>
        {
            Console.WriteLine($"PAIRING-REQUESTED kind={e.PairingKind} -> Accept");
            e.Accept();
        };

        var result = await custom.PairAsync(
            DevicePairingKinds.ConfirmOnly,
            DevicePairingProtectionLevel.Encryption);

        Console.WriteLine($"PAIR-STATUS: {result.Status}");
        return result.Status == DevicePairingResultStatus.Paired ? 0 : 1;
    }
}
