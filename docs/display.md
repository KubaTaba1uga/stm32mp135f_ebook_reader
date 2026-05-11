# Display

The display we choose is:
[https://www.waveshare.com/product/displays/e-paper/7.8inch-e-paper.htm](https://www.waveshare.com/product/displays/e-paper/7.8inch-e-paper.htm)

We tried Chipper displays, but the main problem was resolution. On resolutions like `800x480`, displaying PDFs properly proved to be impossible without reflowing the pdf, when we take photo of the page we need to scale it down to a low resolution which make PDFs unreadable.

## Pinout

| Connector pin | GPIO bank | GPIO pin | Alternate func (T/F) | Alt func settings | GPIO Settings       | Role                             |
|---------------|-----------|----------|----------------------|-------------------|---------------------|----------------------------------|
| 25            | —         | —        | F                    | —                 | —                   | Ground                           |
| 21            | E         | 4        | T                    | SPI5_MISO         | Output, Active high | DOUT (SPI data from HAT)         |
| 19            | H         | 12       | T                    | SPI5_MOSI         | Output, Active high | DIN (SPI data to HAT)            |
| 23            | G         | 10       | T                    | SPI5_SCK          | Output, Active high | CLK (SPI clock)                  |
| 24            | H         | 11       | T                    | SPI5_NSS          | Output, Active low  | CS (SPI chip select; active low) |
| 29            | B         | 10       | F                    | —                 | Output, Active low  | RST (reset)                      |
| 31            | A         | 7        | F                    | —                 | Input, Active low   | BUSY (device ready/busy)         |

```{note}
Connector pin stands for the pin number on the 40-pin hardware header.
```

```{note}
When the `DC` pin is pulled HIGH, the data will be interpreted as data.
When the pin is pulled LOW, the data will be interpreted as a command.
```

```{note}
When the `BUSY` pin is pulled HIGH, the device is idle.
When the pin is pulled LOW, the device is busy processing a command and should not be communicated with.
```
