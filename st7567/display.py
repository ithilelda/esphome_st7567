from esphome import pins
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, spi
from esphome.const import CONF_ID, CONF_LAMBDA, CONF_WIDTH, CONF_HEIGHT, CONF_DC_PIN, CONF_RESET_PIN, CONF_INVERT

AUTO_LOAD = ["display"]
CODEOWNERS = ["@ithilelda"]
DEPENDENCIES = ["spi"]

st7567_ns = cg.esphome_ns.namespace("st7567")
ST7567 = st7567_ns.class_(
    "ST7567", cg.PollingComponent, display.DisplayBuffer, spi.SPIDevice
)
ST7567Ref = ST7567.operator("ref")

CONF_FLIP_X = "flip_x"
CONF_FLIP_Y = "flip_y"
CONF_OFFSET_X = "offset_x"
CONF_OFFSET_Y = "offset_y"
CONF_REG_RATIO = "reg_ratio"
CONF_EV_VALUE = "ev_value"
CONF_BIAS_HIGH = "bias_high"
CONF_BOOSTER_HIGH = "booster_high"

CONFIG_SCHEMA = (
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ST7567),
            cv.Required(CONF_DC_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_WIDTH, default=128): cv.int_range(min=0, max=132),
            cv.Optional(CONF_HEIGHT, default=64): cv.int_range(min=0, max=64),
            cv.Optional(CONF_FLIP_X, default=True): cv.boolean,
            cv.Optional(CONF_FLIP_Y, default=False): cv.boolean,
            cv.Optional(CONF_OFFSET_X, default=0): cv.int_range(min=-64, max=64),
            cv.Optional(CONF_OFFSET_Y, default=0): cv.int_range(min=-32, max=32),
            cv.Optional(CONF_REG_RATIO, default=4): cv.int_range(min=0, max=7),
            cv.Optional(CONF_EV_VALUE, default=31): cv.int_range(min=0, max=63),
            cv.Optional(CONF_BIAS_HIGH, default=False): cv.boolean,
            cv.Optional(CONF_BOOSTER_HIGH, default=False): cv.boolean,
            cv.Optional(CONF_INVERT, default=False): cv.boolean,
            cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.polling_component_schema("1s"))
    .extend(spi.spi_device_schema())
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)

    dc = await cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc))

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
    cg.add(var.set_flip_x(config[CONF_FLIP_X]))
    cg.add(var.set_flip_y(config[CONF_FLIP_Y]))
    cg.add(var.set_offset_x(config[CONF_OFFSET_X]))
    cg.add(var.set_offset_y(config[CONF_OFFSET_Y]))
    cg.add(var.set_reg_ratio(config[CONF_REG_RATIO]))
    cg.add(var.set_ev(config[CONF_EV_VALUE]))
    cg.add(var.set_bias(config[CONF_BIAS_HIGH]))
    cg.add(var.set_booster(config[CONF_BOOSTER_HIGH]))
    cg.add(var.set_inverted(config[CONF_INVERT]))

    await display.register_display(var, config)