MPU
===

.. note::
   - `Development kit page <https://www.st.com/en/evaluation-tools/stm32mp135f-dk.html>`_
   - `Development kit datasheet <https://www.st.com/resource/en/data_brief/stm32mp135f-dk.pdf>`_
   - `Development kit manual <https://www.st.com/resource/en/user_manual/um2993-discovery-kit-with-1-ghz-stm32mp135fa-mpu-stmicroelectronics.pdf>`_
   - `CPU reference manual <https://www.st.com/resource/en/reference_manual/rm0475-stm32mp13xx-advanced-armbased-32bit-mpus-stmicroelectronics.pdf>`_

Our e-book reader uses the STM32MP135F as its main processor. I selected it to get hands-on experience with the STM32 MPU family, and because it is one of the simplest devices in the lineup. The performance requirements are modest for an e-book reader, so a single core running up to 900 MHz is more than sufficient.

As CPU carrier we chose ST's STM32MP135F-DK development kit. The board is much larger and more feature-rich than what we would include in the final product, but it provides everything needed to validate the software stack, display pipeline, power modes, and overall system architecture. In the next hardware iteration we would design a smaller, purpose-built board that keeps only the required peripherals and matches the intended enclosure and power constraints.

.. TO-DO: migrate to some small SBC and add config for it and add sth like: intially we were using stm32mp135f-dk for our carrier board but blablabla.
