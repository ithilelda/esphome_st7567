from esphome import pins
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, spi
from esphome.const import CONF_ID, CONF_LAMBDA, CONF_WIDTH, CONF_HEIGHT, CONF_DC_PIN, CONF_RESET_PIN

AUTO_LOAD = ["display"]
CODEOWNERS = ["@ithilelda"]
DEPENDENCIES = ["spi"]

st7567_ns = cg.esphome_ns.namespace("st7567")
ST7567 = st7567_ns.class_(
    "ST7567", cg.PollingComponent, display.DisplayBuffer, spi.SPIDevice
)
ST7567Ref = ST7567.operator("ref")

CONFIG_SCHEMA = (
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ST7567),
            cv.Required(CONF_WIDTH): cv.int_,
            cv.Required(CONF_HEIGHT): cv.int_,
            cv.Required(CONF_DC_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(spi.spi_device_schema())
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)

    if CONF_RESET_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(reset))
    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(ST7567Ref, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
    cg.add(var.set_width(config[CONF_WIDTH]))
    cg.add(var.set_height(config[CONF_HEIGHT]))
    dc = await cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc))

    await display.register_display(var, config)