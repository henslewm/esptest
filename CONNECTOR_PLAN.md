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
