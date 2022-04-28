# Initialize all required esp32 berry-files
core.load("esp32/wifi.be")
core.load("esp32/bluetooth.be")
core.load("esp32/ota.be")
core.load("esp32/i2s.be")
core.load("esp32/gpio.be")
core.load("esp32/i2c.be")
core.load("esp32/led_strip.be")
core.load("esp32/boards.be")
core.load("esp32/base_driver.be")
core.load("esp32/hardware_plugin.be")

# drivers
core.load("esp32/dacs/es8388_driver.be")
core.load("esp32/dacs/ac101_driver.be")
core.load("esp32/dacs/tas5711_driver.be")
core.load("esp32/dacs/ma12070p_driver.be")
core.load("esp32/dacs/jp2137_driver.be")
core.load("esp32/dacs/i2s_driver.be")
core.load("esp32/dacs/dummy_driver.be")
