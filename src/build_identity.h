#pragma once

// Prints the running firmware's identity, including the ELF SHA-256 that the
// build records, so hardware evidence can be tied to an exact binary
// (docs/PLATFORMIO.md, .claude/rules/05-modular-code.md).
void printBuildIdentity();
