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
