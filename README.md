# ST7567 Display (128x64 lcd display) Component for ESPHome

The ``st7567`` display platform allows you to use
ST7567 ([Taobao](<https://item.taobao.com/item.htm?spm=a21n57.1.0.0.239a523cIT1eXy&id=691384985925&ns=1&abbucket=0#detail>))
displays with ESPHome.

Connect the pins according to the item description. ``CS`` to cs_pin, ``RST`` to reset_pin,  ``A0`` to dc_pin,
``SCK`` to clk_pin, and finally ``SDA`` to mosi_pin.

```yaml
# Example configuration entry
spi:
    clk_pin: GPIO0
    mosi_pin: GPIO3

display:
    - platform: st7567
      cs_pin: GPIO4
      dc_pin: GPIO5
      reset_pin: GPIO1
      lambda: |-
        it.print(0, 0, id(font), "Hello World!");
```

## Configuration variables:

- **cs_pin** (**Required**, :ref:`Pin Schema <config-pin_schema>`): The Chip Select (CS) pin.
- **dc_pin** (**Required**, :ref:`Pin Schema <config-pin_schema>`): The A0 pin.
- **reset_pin** (*Optional*, :ref:`Pin Schema <config-pin_schema>`): The RESET pin. Defaults to not connected.
- **rotation** (*Optional*): Set the rotation of the display. Everything you draw in ``lambda:`` will be rotated
  by this option. One of ``0°`` (default), ``90°``, ``180°``, ``270°``.
- **width** (*Optional*, int): Width of the display. Numbers are only allowed between ``0~132``. Defaults to ``128``.
- **height** (*Optional*, int): Height of the display. Numbers are only allowed between ``0~64``. Defaults to ``64``.
- **flip_x** (*Optional*, boolean): Flip the horizontal axis on the screen. Defaults to ``true``.
- **flip_y** (*Optional*, boolean): Flip the vertical axis on the screen. Defaults to ``false``.
- **offset_x** (*Optional*, int): Set this option if some horizontal pixel is missing. Numbers are only allowed between ``-64~64``. Defaults to ``0``.
- **offset_y** (*Optional*, int): Set this option if some vertical pixel is missing. Numbers are only allowed between ``-32~32``. Defaults to ``0``.
- **reg_ratio** (*Optional*, int): Set this option and ev_value to setup contrast. Numbers are only allowed between ``0~7``. Defaults to ``0``.
- **ev_value** (*Optional*, int): Set this option and reg_ratio to setup contrast. Numbers are only allowed between ``0~63``. Defaults to ``0``.
  A middle value is recommended by the datasheet.
- **invert** (*Optional*, boolean): Invert all pixel state on the display. Defaults to ``false``.
- **bias_high** (*Optional*, boolean): If set to true, the bias will be 1/7, otherwise it will be 1/9. Defaults to ``false``.
- **booster_high** (*Optional*, boolean): If set to true, the booster will be x5, otherwise it will be x4. Defaults to ``false``.
- **lambda** (*Optional*, :ref:`lambda <config-lambda>`): The lambda to use for rendering the content on the display.
  See :ref:`display-engine` for more information.
- **update_interval** (*Optional*, :ref:`config-time`): The interval to re-draw the screen. Defaults to ``1s``.
- **pages** (*Optional*, list): Show pages instead of a single lambda. See :ref:`display-pages`.
- **spi_id** (*Optional*, :ref:`config-id`): Manually specify the ID of the :ref:`SPI Component <spi>` if you want
  to use multiple SPI buses.
- **id** (*Optional*, :ref:`config-id`): Manually specify the ID used for code generation.