# Testing (ohne Kamera)

```bash
npm test   # node --test test/smoke.test.js test/api-surface.test.js
```

- `test/smoke.test.js` — Versionen, `isMock()`, `enumerateDevices() === []`,
  `grabMock`-Muster.
- `test/api-surface.test.js` — prüft, dass alle in `types/index.d.ts`
  exportierten Funktionen in `lib/index.js` existieren und die
  dokumentierten Signaturen einhalten (Rückgabetypen, Buffer-Länge).

Beide Tests brauchen weder Kamera noch pylon-SDK (Mock-Build genügt).
