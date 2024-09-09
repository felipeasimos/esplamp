# Esp Lamp

A simple esp8266 rgb lamp controllable by an API.

## How to Run

1. Install `PlatformIO`
2. Create a `.env` based on `env_example`
3. Plug a esp8266 to your computer
4. `pio run -t upload -t monitor`
    * get the server IP from here
5. Use `curl` or another kind of REST client to communicate with the API
    * `GET` -> get current RGB values
    * `POST` -> set new RGB values

### Roadmap

- [x] Change colors with PWM
- [x] Connect to wifi
- [x] Accept TCP connection
- [x] Respond TCP connection
- [x] Optional `POST` parameters
    - [x] duration of fade effect to new RGB color
- [ ] `GET` `pwm_step` and `rgb`
- [ ] return `pwm_step` and `rgb` in `POST`
- [ ] Graceful request shutdown and response
