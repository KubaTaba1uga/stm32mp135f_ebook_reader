MPU
===

.. note::
   - `Development kit page <https://www.st.com/en/evaluation-tools/stm32mp135f-dk.html>`_
   - `Development kit datasheet <https://www.st.com/resource/en/data_brief/stm32mp135f-dk.pdf>`_
   - `Development kit manual <https://www.st.com/resource/en/user_manual/um2993-discovery-kit-with-1-ghz-stm32mp135fa-mpu-stmicroelectronics.pdf>`_
   - `CPU reference manual <https://www.st.com/resource/en/reference_manual/rm0475-stm32mp13xx-advanced-armbased-32bit-mpus-stmicroelectronics.pdf>`_

An e-book reader does not require much computing power. Its main workload is rendering PDF pages, converting them into images, and sending those images to the display. For this reason, we selected the **STM32MP135F (single-core, 1 GHz)** from ST.

Optimizing for long battery life should be easier on this platform, mainly due to ST’s strong documentation and well-supported power-management features (in contrast to platforms such as Raspberry Pi). If the project evolves into a product, there are also multiple **System-on-Module (SoM)** options available, which can significantly reduce hardware development effort.

We developed the initial prototype using the **STM32MP135F-DK** development kit. This board is much larger and more feature-rich than what we would include in a final device, but it provides everything needed to validate the software stack, display pipeline, power modes, and overall system architecture. In the next hardware iteration, we would design a smaller, purpose-built board that includes only the required peripherals and matches the intended enclosure and power constraints.

We are also considering replacing the STM32MP135F-DK with a Raspberry Pi–form-factor SBC. This could simplify mechanical design and enclosure development, and it is worth evaluating as an alternative platform.

.. TO-DO: migrate to some small SBC and add config for it and add sth like: intially we were using stm32mp135f-dk for our carrier board but blablabla.

