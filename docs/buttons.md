# Buttons

Connect buttons according to the table below:

| BTN   | GPIO | PIN | Is working |
| ----- | ---- | --- | ---------- |
| menu  | B1   | 7   |            |
| enter | D4   | 11  |            |
| down  | D13  | 13  |            |
| left  | E14  | 16  |            |
| right | D15  | 10  |            |
| up    | C0   | 12  |            |

```{note}
If you are wondering why we choose pins in this weird pattern instead just next to each other,
the reason is interrupt multiplexing. The EXTI uses one interrupt line for each gpio
pin of the same offset. So if we would use PA0 and PC0, one of interrupts assigned to these
pins wouldn't work.
```

Once buttons are connected run following command and press all buttons to confirm they are working properly:
```bash
evtest
```

