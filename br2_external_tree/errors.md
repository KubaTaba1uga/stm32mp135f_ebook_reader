## 1) Fix the **UCSI STM32G0 firmware missing**

You need the file named exactly:

* `stm32g0-ucsi.mp135f-dk.fw`

Put it into your target rootfs at:

* `/lib/firmware/stm32g0-ucsi.mp135f-dk.fw`

Buildroot ways:

* Add it via a **rootfs overlay** (`board/<yourboard>/rootfs-overlay/lib/firmware/...`)
* Or install it from a custom package / post-build script

If you **don’t use USB-C/PD**, disable that firmware/driver path in your kernel config or device tree so it stops probing.

## 2) Fix **“Starting network … RTNETLINK answers: File exists”**

This is almost always an init script trying to add an IP/route that already exists (commonly because the kernel cmdline `ip=...` already configured `usb0`).

Quick fixes (pick one):

* **Make init idempotent** (best):

  * before configuring, do:

    * `ip addr flush dev usb0`
    * `ip route flush dev usb0`
  * or ignore that error:

    * `ip ... || true`
* **Or remove `ip=...` from kernel cmdline** and let userspace init configure it.

If you paste your network init snippet (the part that runs `ip addr add` / `ip route add`), I’ll mark the exact line and the minimal patch.

## 3) Reduce noise: **mmc1: Failed to initialize a non-removable card**

If mmc1 is unused:

* disable that controller node in DT (status = "disabled") or fix the DT wiring/regulator if it’s supposed to exist.

## 4) Non-fatal DT warnings (optional cleanup)

* `/cpus/cpu@0 missing clock-frequency`: usually safe to ignore.
* `dwc2 ... using dummy regulator`: better DT hygiene—define `vusb_a` / `vusb_d` regulators if you want correct power modeling.

## 5) RTC time = 2000-01-01

Not fatal, but it will break TLS and make logs confusing.

* If you have an RTC + battery: fix wiring / DT and ensure driver enabled.
* If not: sync time from network early (NTP) or set time in init.

If you paste:

* your kernel cmdline (the full `bootargs`)
* and the network init script section that configures `usb0`
  …I can give you a concrete one-shot patch that removes the “File exists” failure without changing your overall behavior.



TO-DO:

1. Fix the **UCSI STM32G0 firmware missing** - DONE
2. Fix **“Starting network … RTNETLINK answers: File exists”**
3. Reduce noise: **mmc1: Failed to initialize a non-removable card**
4. Non-fatal DT warnings (optional cleanup)
5. RTC time = 2000-01-01
