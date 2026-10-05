# Bootstrap Foundation Review

State: AWAITING_APPROVAL — autonomy is OFF until explicit activation.
Profile: software-hardware
Architecture fingerprint: `e702f64de79f8c542af1b1f9920443080e3a4aac8a6eb3baa4325dad30112843`

## Charter

```json
{
  "name": "esp32test",
  "objective": "To develop firmware on an ESP32S3 Zero Waveshare that attaches as HID keyboard & mouse when plugged into USB and then automatically links as a BLE Keyboard.",
  "definition_of_done": [
    "No user input needed, BLE keyboard visible in settings and USB Key/mouse visible in device manager, test.txt file created in directory wwith input from both keyboards with date/timestamps typeed out only."
  ],
  "non_goals": [
    "Does not capture, read, log, store, or forward host keyboard, mouse, or other input; the device only emits keystrokes it generates",
    "No WiFi, networking, cloud, or remote-control functionality",
    "Not a general-purpose or persistent input-automation tool beyond the timestamp demonstration"
  ],
  "constraints": [
    "lowest cost, not worried about security since it's on my own machine"
  ]
}
```

## Architecture and milestone dependency graph

```json
{
  "summary": "Single-MCU firmware for a Waveshare ESP32-S3-Zero, built with PlatformIO/Arduino-ESP32 and split into single-responsibility modules indexed by MODULES.md. On USB connection the device enumerates as a composite TinyUSB HID device exposing both a keyboard and a mouse, visible in Windows Device Manager. In parallel it advertises as a BLE HID keyboard that Windows bonds with once and auto-reconnects thereafter, visible in Bluetooth settings. With no steady-state user input, a scheduler/business-logic module emits keystrokes that type date/timestamped lines into a text file on the host over both the USB and BLE keyboard paths, demonstrating each transport. The device only emits keystrokes it generates; it never reads, captures, logs, or forwards host input. Timestamps come from firmware build time plus uptime, the lowest-cost option requiring no network or credentials.",
  "boundaries": [
    "Hardware abstraction boundary: a device/HID layer wraps USBHIDKeyboard, USBHIDMouse, and the BLE keyboard behind a keystroke-emitter contract that a host-side fake can satisfy, so business logic is testable without the board.",
    "USB mode: HID requires TinyUSB (ARDUINO_USB_MODE=0, USB-OTG). The S3-Zero has one USB-C port and no UART bridge, so HID, flashing, and serial monitor share it; the OTG enumeration identity differs from the 303A:1001 CDC default and must be read from the pinned core during execution. BOOT-button recovery applies.",
    "BLE exposes a single HID input report, keyboard only; the mouse is USB-only. This sidesteps the core-3.3.12 NimBLE duplicate-UUID report bug recorded in docs/BLE_NOTES.md.",
    "Transports in scope are USB HID and BLE HID only; no WiFi, networking, or cloud.",
    "Host file creation targets a device-driven keystroke macro (launch Notepad, type, save test.txt) to satisfy no-user-input; a pre-opened editor is the documented fallback if the macro proves unreliable.",
    "Changing USB mode, protocol/report maps, hardware support scope, the HID abstraction boundary, persistence, or the core/framework requires renewed approval."
  ],
  "milestones": [
    "Baseline and hardware identification",
    "USB HID composite keyboard and mouse",
    "BLE keyboard link and pairing",
    "Timestamp and test.txt typing workflow",
    "Hardware-in-loop verification"
  ],
  "dependencies": [
    "Baseline and hardware identification -> USB HID composite keyboard and mouse",
    "Baseline and hardware identification -> BLE keyboard link and pairing",
    "USB HID composite keyboard and mouse -> Timestamp and test.txt typing workflow",
    "BLE keyboard link and pairing -> Timestamp and test.txt typing workflow",
    "Timestamp and test.txt typing workflow -> Hardware-in-loop verification"
  ]
}
```

## Sources and evidence map

```json
[
  "./"
]
```

## Risks

```json
[
  "Compile or simulation success may diverge from physical hardware; VERIFIED_ON_HARDWARE is earned only by operator attestation, never authored.",
  "USB-OTG/HID mode changes the enumeration identity and shares the single USB-C port with flashing and monitoring, risking lockout; mitigated by BOOT-button recovery and reading the OTG VID:PID from the pinned core.",
  "BLE HID on Windows needs finicky one-time pairing (docs/BLE_NOTES.md); mitigated by scoping BLE to one keyboard report and accepting one-time operator pairing.",
  "The device-driven Notepad macro is timing- and focus-sensitive; mitigated by a pre-opened-editor fallback.",
  "Build-time-plus-uptime timestamps drift and reset on reboot; accepted under the lowest-cost constraint.",
  "Arduino-ESP32 core 3.x NimBLE can silently drop a duplicate-UUID HID report; mitigated by a keyboard-only BLE report map."
]
```

## Routing and cost policy

```json
{
  "policy": "Assign the cheapest capability tier expected to satisfy each packet's acceptance criteria; codecs, scheduler logic, fakes, and host-side tests route to local tiers first, while the USB/BLE hardware adapter and integration packets carry high risk per DOMAIN_PROFILE.md and add model and cross-family review. Escalate only on objective failure or risk, never preference."
}
```

## GitHub workflow and routine permissions

```json
{
  "policy": "One bounded issue per packet in a dependency-ordered wave plan; each PR links its packet and carries validation evidence; an automated Codex or CodeRabbit review matching the head commit (ADR-074) precedes any merge; accept only inside the approved foundation. GitHub stays read-only: pushing branches, opening issues/PRs, and merging are reserved human actions pending explicit authorization."
}
```

## Reserved human actions

```json
[
  "Material architecture change (USB mode, protocol/report maps, hardware scope, HID abstraction boundary, persistence, or framework)",
  "Physical-hardware actions that risk damaging the board (driving GPIO, power, or peripherals outside spec)",
  "Any claim of hardware verification: VERIFIED_ON_HARDWARE is earned by operator attestation of observed behavior, never authored by the agent",
  "GitHub writes: push, open issue or PR, merge",
  "One-time Windows BLE pairing of the device",
  "Consequential external action"
]
```

## Domain orientation

```json
{
  "baseline": "No firmware exists yet; the repository holds control-plane and template files only, with no src/ or platformio.ini. This is a greenfield firmware build.",
  "hardware_identity": "Waveshare ESP32-S3-Zero: ESP32-S3 SoC, single USB-C port, no onboard UART bridge, onboard WS2812 RGB LED. Exact chip variant, flash size, and PSRAM are datasheet-derived pending confirmation via esptool flash-id; Waveshare lists ESP32-S3FH4R2 with 4MB flash and 2MB in-package PSRAM.",
  "interfaces": "USB as a TinyUSB HID composite (keyboard plus mouse) and BLE as an HID keyboard. No WiFi or other networking.",
  "specifications": "USB HID class spec and HID Usage Tables; Espressif ESP32-S3 technical reference and TinyUSB; Arduino-ESP32 USB and BLE APIs; Waveshare ESP32-S3-Zero wiki. Tracked in SOURCE_INDEX.md and not all yet verified.",
  "environment": "Windows 11 with PowerShell 7.6; PlatformIO CLI and Python 3.12 installed; the host is both the HID/BLE target and the Device Manager and Bluetooth-settings verifier.",
  "known_paths": "Known-good: host-side unit, contract, and simulation rungs run without the board via a keystroke-emitter fake, the scheduler, and the timestamp formatter. Known-failing: none observed yet, because no firmware has been built or run on hardware.",
  "validation_resources": "Hardware-in-loop: the ESP32-S3-Zero connected over USB-C to the Windows host, with the operator observing Device Manager, Bluetooth settings, and the typed test.txt. Host-side fakes and loopback cover the first five ladder rungs.",
  "physical_access": "The project owner (operator) has physical access to plug in the board and press BOOT; autonomous workers do not. Hardware runs are operator actions.",
  "architecture_boundaries": "Changes to USB mode or HID composite layout, the BLE report map, the hardware abstraction boundary, transport scope such as adding WiFi, the persistence approach, or the Arduino-ESP32 core/framework require renewed approval."
}
```

## Unresolved blockers

```json
[]
```

## Project configuration and disclosed defaults

```json
{
  "domain_profile": "software-hardware",
  "project_name": "esp32test",
  "objective": "To develop firmware on an ESP32S3 Zero Waveshare that attaches as HID keyboard & mouse when plugged into USB and then automatically links as a BLE Keyboard.",
  "success_criteria": [
    "No user input needed, BLE keyboard visible in settings and USB Key/mouse visible in device manager, test.txt file created in directory wwith input from both keyboards with date/timestamps typeed out only."
  ],
  "out_of_scope": [
    "Does not capture, read, log, store, or forward host keyboard, mouse, or other input; the device only emits keystrokes it generates",
    "No WiFi, networking, cloud, or remote-control functionality",
    "Not a general-purpose or persistent input-automation tool beyond the timestamp demonstration"
  ],
  "constraints": [
    "lowest cost, not worried about security since it's on my own machine"
  ],
  "source_locations": [
    "./"
  ],
  "owner": "Winston",
  "risk_tier": "low",
  "sensitivity": "private",
  "hardware_in_scope": true,
  "domain": "software-hardware",
  "project_slug": "esp32test",
  "problem_statement": "To develop firmware on an ESP32S3 Zero Waveshare that attaches as HID keyboard & mouse when plugged into USB and then automatically links as a BLE Keyboard.",
  "deliverables": [
    "Project-specific analysis or implementation",
    "Current handoff and state records"
  ],
  "ai_clients": [
    "chatgpt",
    "codex",
    "claude",
    "claude-code",
    "mistral"
  ],
  "connectors": [
    "github",
    "web"
  ],
  "repeatable_workflows": [],
  "output_formats": [
    "markdown"
  ],
  "target_date": "",
  "jurisdiction_or_version": "",
  "connector_permissions": {
    "github": "read",
    "web": "read"
  },
  "template_mode": false,
  "template_version": "1.0.0",
  "created": "2026-10-04"
}
```

## Bound document hashes

```json
{
  "PROJECT_CHARTER.md": "78742af77bd7fc449d2d97593d501dd09c0fe7aff3d929770135adef71fb67a6",
  "CONNECTOR_PLAN.md": "8d8fbd6a863c988f1d4c94ef32bf999a793171f964c86669365b56c03f2b8a5d",
  "SKILL_PLAN.md": "322e9e54e2ee1850ca77aee547e1616c3bea7437f3b3928446dbdbee74164fe6",
  "DOMAIN_PROFILE.md": "91d42727ef1a1b26894f5c3e9f6938c01ac8e372544079da97b7140665875e65"
}
```

## Readiness

- Ready for an explicit user decision.

## Bound document: PROJECT_CHARTER.md

# Project Charter

## Project

- **Name:** esp32test
- **Slug:** esp32test
- **Domain:** software-hardware
- **Version:** Not specified
- **Risk tier:** low
- **Sensitivity:** private
- **Owner:** Winston
- **Target date:** Not fixed

## Problem statement

To develop firmware on an ESP32S3 Zero Waveshare that attaches as HID keyboard & mouse when plugged into USB and then automatically links as a BLE Keyboard.

## Desired outcome

To develop firmware on an ESP32S3 Zero Waveshare that attaches as HID keyboard & mouse when plugged into USB and then automatically links as a BLE Keyboard.

## Definition of done

- No user input needed, BLE keyboard visible in settings and USB Key/mouse visible in device manager, test.txt file created in directory wwith input from both keyboards with date/timestamps typeed out only.

## Required deliverables

- Project-specific analysis or implementation
- Current handoff and state records

## Scope

### In scope

- Work necessary to achieve the desired outcome and deliverables.

### Out of scope

- Does not capture, read, log, store, or forward host keyboard, mouse, or other input; the device only emits keystrokes it generates.
- No WiFi, networking, cloud, or remote-control functionality.
- Not a general-purpose or persistent input-automation tool beyond the timestamp demonstration.

## Constraints and approval gates

- lowest cost, not worried about security since it's on my own machine

- Preserve authoritative source material and provenance.
- Keep secrets and unapproved restricted material out of Git.
- Require explicit approval for consequential external writes.

## Decision rights

- The user owns goals, scope, business choices, and consequential external actions.
- AI tools may research, analyze, draft, organize, validate, and make reversible repository changes within granted permissions.
- Material adverse facts, conflicts, and high-impact assumptions must be surfaced.


## Bound document: CONNECTOR_PLAN.md

# Connector Plan

## Selected connectors

| Connector / source | Use | Scope | Permission posture |
|---|---|---|---|
| github | Repository state, issues, history, and project files | Project repo | read; No force push; project writes only when requested |
| web | Current public facts and primary authority | Public sources | read; Read only |

## Rules

- Use only connectors selected above unless the user approves an addition.
- Prefer the narrowest useful search or folder/date scope.
- Record material sources in `SOURCE_INDEX.md`.
- Availability is not authority for a consequential write.
- Sends, filings, publishing, purchases, deletions, permission changes, and other external modifications require explicit current instruction.


## Bound document: SKILL_PLAN.md

# Skill Plan

## Selected and candidate skills

| Skill | Type | Reason | Status |
|---|---|---|---|
| complex-project-bootstrapper | Included custom skill | Initialize or retrofit this project consistently | Review / enable when needed |

## Decision rule

Enable an existing trusted skill when it fits the workflow. Create a custom skill only when inputs, outputs, connector needs, and a repeatable procedure are concrete. Review source and permissions before installing a public skill.


## Bound document: DOMAIN_PROFILE.md

# Domain Profile — Software + Hardware Interfaces

This is the default `main` specialization of the universal autonomous project template: software
that interacts with physical hardware, built so that most bounded work can go to cheap or local
models while no claim of hardware success is ever made on the strength of compilation or
simulation. `AUTONOMY_CONTROL_PLANE.md` is the controlling workflow policy; the controllers named
below are the mechanism.

## Startup gate

Autonomy is OFF until interactive intake is complete and the user approves the exact package.
`scripts/validate_bootstrap.py` requires, for this profile, a meaningful answer to every domain
orientation field before review or activation: existing baseline; exact hardware identity and
firmware; interfaces and protocols; authoritative specifications; host and deployment
environment; known-good and known-failing paths; fixtures, simulators, loopback and
hardware-in-loop resources; physical-access constraints; and the architecture boundaries whose
change requires approval. The architect then presents the component boundaries, dependency graph,
hardware abstraction boundary, verification ladder per component, routing policy, first milestone
and stop conditions. A placeholder answer is refused, not deferred.

## Decomposition

Prefer components that can be validated without physical hardware. Every packet names exactly one
component from the profile's separation list: protocol codec, transport, device abstraction,
hardware adapter, configuration, persistence, business logic, UI/API, simulator/fake,
telemetry/logging, deployment/operations. A packet that spans several is not independently
testable and is decomposed further. Hardware-facing components expose contracts that a fake or
loopback can satisfy, so their host-side rules are machine-runnable even when the device is not
on the bench. `examples/software-hardware/` is a complete worked decomposition.

## Work packet

The common contract is unchanged. For this profile the `domain` block is structural, validated by
`config/domains/software-hardware.schema.json` and `scripts/software_hardware.py` wherever a
contract is validated:

- `component`: the one separation category.
- `hardware_assumptions`: every physical behavior the packet relies on, each citing a contract
  source. Empty means the packet is `NOT_HARDWARE_FACING`.
- `protocol_references`: the specification sources implemented against; required when any
  hardware assumption exists.
- `validation_levels`: every validation id mapped to exactly one rung of the ladder.
- `hardware_status`: `UNVERIFIED_ON_HARDWARE` or `NOT_HARDWARE_FACING`. A contract cannot declare
  `VERIFIED_ON_HARDWARE`; it is earned, never authored. The block is closed — no undeclared key
  and no free text can carry that literal anywhere in it — and `validate_contract` takes the
  profile as a required argument, so no caller skips these rules by omitting it.

## Verification ladder

static → unit → contract → simulation/loopback → integration → hardware-in-loop → representative
field workflow where required.

The first five rungs are machine-runnable: each such validation must declare a `command`, and the
acceptance controller's deterministic gate re-executes it in the reviewed workspace and records
what it observed (ADR-014). The two hardware rungs must not declare a command: they fail closed to
an attestation by a non-implementer operator, and that attestation must bind a structured hardware
evidence record by digest. A hardware rung with a command is refused as a simulator masquerading
as hardware; a machine rung without one is refused as attestation substituting for a runnable
check. `software_hardware.py status` derives the earned status from the ledger:
`VERIFIED_ON_HARDWARE` only for an accepted task whose hardware rungs were all attested and whose
contract does not declare `domain.synthetic: true` — a synthetic contract's inputs are fictional
by that declaration, so its hardware rungs can be attested and their bound records re-verified,
but the derived status stays `UNVERIFIED_ON_HARDWARE` regardless; every worked example in this
template declares it. A compile or simulation pass never upgrades it. The output names its
`evidence_basis`: without
`--evidence-dir` the status rests on the ledger's attestations and their declared digests, and
says it is attested, not record-verified; with `--evidence-dir` every bound record is found by
digest, validated as a hardware evidence record, and re-verified for task, validation, rung, a
`pass` outcome, the attesting operator and every line the attestation carried, compared exactly
except for the operator — a digest proves which bytes were bound, not that they are a record or
that they say what was attested. A ledger stored before these rules existed replays marked with
its shortfall for audit and acceptance status; only new events are refused, and the hardware
status is the one derivation that refuses (ADR-032).

## Code structure

Keep firmware in single-responsibility modules indexed by a `MODULES.md` beside the sources
(template: `templates/software-hardware/MODULES.md`). The rules are in
`.claude/rules/05-modular-code.md`.

## Build configuration (Arduino / ESP32)

Every Arduino or ESP32 firmware project keeps a root `platformio.ini` generated under
`docs/PLATFORMIO.md`. BLE/NimBLE and Windows-pairing lessons: `docs/BLE_NOTES.md`.

## Cost posture

Pure functions, codecs, parsers, fixtures, fakes, tests, log analysis and repetitive adapters route
to the local tiers first; objective failures and risk, never preference, drive escalation. The
hardware adapter and the integration/field packets carry `high` risk so the acceptance floor adds
independent model review and the cross-family gate. Hardware runs are operator actions and are
never dispatched to a worker.

## GitHub ledger

One tracking issue per milestone with its dependency-ordered wave plan; one issue per packet;
new findings become separate issues; PRs link the packet and carry validation evidence. Hardware
observations are recorded as bound evidence records and their attestations, not as prose in a
comment.

## Human-intervention gate

Routine implementation, testing, issue/PR updates, model escalation and bounded refactoring
proceed autonomously inside the approved architecture. Stop for user approval before materially
changing protocol assumptions, hardware support scope, public interfaces, persistence
architecture, the security/authentication model, deployment topology, core framework/language, or
the hardware abstraction boundary. Any action on physical hardware that can damage it, or any
claim of hardware verification, is an operator action recorded as evidence, never an autonomous
step.

