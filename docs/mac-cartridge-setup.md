# Chromatic DevDay cartridge setup on Mac

Verified September 29, 2026. Develop Rollback on Mini1; download each release to Zach’s remote MacBook Pro and connect the Chromatic to that Mac for cartridge writing. The v0.1.0 training ROM is built and emulator-verified. It has not been flashed to hardware.

## Official DevDay instructions

The [ModRetro DevDay Edition Quickstart Guide](https://support.modretro.com/en_us/chromatic-devday-edition-quickstart-guid-By1iOlcMg) documents this sequence:

1. Install [MR Updater](https://support.modretro.com/en_us/chromatic-firmware-updater-ryhoYnzCx) and follow its firmware update instructions.
2. In MR Updater on the MacBook Pro, press **Command-I** and enter the developer activation code supplied with the DevDay kit. Activation enables cartridge writing on that computer.
3. In Codex desktop on the MacBook Pro, open **Plugins**, search **ModRetro Chromatic**, and install it. The guide describes emulator previews, hardware streaming, and writing a completed ROM to the inserted cartridge.

The plugin is not installed in this task. Its examples use GB Studio. Zach subsequently authorized implementation; the first native build uses GBDK and C. The game runtime choice does not replace this supported cartridge-writing workflow.

If no activation code can be found, use [ModRetro Technical Support](https://modretro.com/contact). Suggested message, not sent: “I received a Chromatic DevDay Edition and rewritable cartridge at OpenAI DevDay 2026, but cannot find the developer activation code. How can I retrieve or obtain it for my Mac?” The public guide does not explain code recovery or specify where inside the kit the code is located.

## Mac download and connection

MR Updater requires macOS 12 or newer. Use its **Apple Silicon** download for an M-series Mac or **Intel** download otherwise. Connect the powered-on Chromatic by USB when prompted. Leave it connected and powered throughout any update and wait for the updater to finish before disconnecting. The same official page documents save backup in Cart Clinic for supported ModRetro cartridges.

For our first hardware session, insert the intended cartridge while the handheld is off, connect a USB data cable to the MacBook Pro, then turn it on. Confirm the detected cartridge before writing. Preserve any existing game or save first; the current CLI does not expose a full-ROM backup command in its top-level help. A blank cartridge’s header alone cannot establish its physical storage or save capability.

## Verified command-line alternative

[ModRetro’s official npm package](https://www.npmjs.com/package/@modretro/chromatic-cli) supports Apple Silicon and Intel macOS. No additional USB-driver installation is needed on macOS. It requires Node.js 18 or newer; use a supported [Node.js LTS release](https://nodejs.org/en/download) if Node/npm is absent.

The following syntax was checked against the actual macOS ARM64 **1.2.1** executable’s help output on Mini1. The binary was downloaded into a temporary directory and its npm SHA-512 integrity checked. Only help commands were run; no activation, device mutation, or flash was performed.

Install on the MacBook Pro:

```sh
npm install --global @modretro/chromatic-cli@1.2.1
chromatic-cli --help
```

If using CLI activation, replace the placeholder with the ModRetro-issued code. The CLI describes activation as enabling features for this computer; GUI activation is the route explicitly documented in the DevDay guide.

```sh
chromatic-cli activate YOUR_MODRETRO_CODE
```

With the device and intended cartridge connected:

```sh
chromatic-cli list-devices
chromatic-cli detect-cart --all
```

After unzipping a Rollback release, open Terminal in that folder. Verify the included checksum file, then use the ROM hash it lists in place of `ACTUAL_RELEASE_SHA256` below:

```sh
shasum -a 256 -c SHA256SUMS
chromatic-cli write-homebrew rollback-e1-training.gbc --expect-sha256 ACTUAL_RELEASE_SHA256
```

Keep the CLI’s confirmation prompts enabled. `--expect-sha256` checks the input file; its help does not establish a cartridge readback guarantee. Record the writer’s actual verification result and then cold-boot and check controls/audio on the handheld. Host-emulated `live-demo` streaming does not prove that the cartridge boots independently.

## Remaining project decisions

- Detect the actual cartridge model/capacity/controller/save support on Zach’s MacBook Pro before setting final build budgets.
- Obtain the issued activation code if it is missing; do not assume a public or universal code.
- The first build uses GBDK/C, a CGB-only header, MBC5, 64 KiB ROM, and no external RAM. Confirm those declarations against the detected physical cart before flashing.
- Release ZIPs contain the ROM, checksum, change notes, verification report, and these instructions. See [release instructions](release-guide.md) and the private repository’s releases for downloadable builds.
