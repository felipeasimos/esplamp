# Esp Lamp

A simple esp8266 rgb lamp controllable by an TCP API.

## How to Run

1. Install `PlatformIO`
2. Create a `.env` based on `env_example`
3. Plug a esp8266 to your computer
4. `pio run -t upload -t monitor`
    * get the server IP from here
5. Get lamp IP from console or monitor UDP broadcast on port 12345 coming from it with the message "openupitsme"
    * I recommend using wireshark to monitor and get a feeling for it!
6. send 6 bytes through TCP to get color/alter color:

```
.-----------------------------------------------------.
| length | P or G |pwn_step|   R    |    G   |   B    |
| 1 byte | 1 byte | 1 byte | 1 byte | 1 byte | 1 byte |
`-----------------------------------------------------´
```

* `length` - always 6.
* `P or G` - `P` to change color and fade effect intensity, `G` to get these information
    * if `G` is used, all the other bytes are ignored, but must still be sent
* `pwm_step` - fade effect intensity. Range from 0 to 100.
* `RGB` fields - range from 0 to 100 each.

7. receive back

```
.-----------------------------------------------------.
| length | status |pwn_step|   R    |    G   |   B    |
| 1 byte | 1 byte | 1 byte | 1 byte | 1 byte | 1 byte |
`-----------------------------------------------------´
```

* `length` - always 6.
* `status` - `1` if OK, `0` on error.
* `pwm_step` - fade effect intensity. Range from 0 to 100.
* `RGB` fields - range from 0 to 100 each.

### Roadmap

- [x] Change colors with PWM
- [x] Connect to wifi
- [x] Accept TCP connection
- [x] Respond TCP connection
- [x] Optional `POST` parameters
    - [x] duration of fade effect to new RGB color
- [x] `GET` `pwm_step` and `rgb`
- [x] return `pwm_step` and `rgb` in `POST`
- [x] respond to discovery broadcast message
- [ ] Qt App
    - [x] discover lamp ip
    - [ ] change color
