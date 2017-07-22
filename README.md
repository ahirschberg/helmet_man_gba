# cool_game_gba
#### (Pending a better name)
The game uses GBA mode 3, which provides bitmapped backgrounds and a sprite object block. The enemies, projectile, player, and runner obstacles are sprites and thus rendered by the GBA hardware, while the background graphic, ground, and text are done via (slowly) copying or drawing pixels directly to the screen. Eventually, I would like to move the gameplay rendering over to a fully tiled mode so I can do parallax scrolling and more intricate backgrounds, as well as a truly moving foreground (currently faking it using two bitmaps offset by half a block, copied each frame using fast 32 bit DMA)

I initially made this for a CS2110 homework project, hence the references to the course in the Makefile. However, I've really enjoyed working on this project and so have continued to update the game with additional features I didn't have time to implement in the week and a half I was given on the homework.

## Screens:

![Start screen](http://i.imgur.com/BAm4onC.png)

#### Running on actual hardware via a flashcart!
<img src="https://lh3.googleusercontent.com/e5oYCnQQTedsPBRn8zoXwi13GjDWHWlRfwzxkCCWbPSMvuaRGA9r59CLtqfESWmhO53x97ig22BomBM9ThHwRaWFE4J7Fy5_yVI0cGDbSGuYfnhymsfDBcosjQokm8a39E-nOvitx79jwcMhlxg1OcgDy1-htRPj4TXmxgfHG9i8On-E5C_-b-xiGE63oGHoBMuJTz1tO5PuidI9le2Mi3vT2Guij9l6eSynkisbsWWtyImC2s3YXEXFsMeG0SIccVHblusTjuLudqFMJf2uyPu2byJaMV_-8iAyuzr20n1Zks2kkoRwqgmn9V1bBdI_9nm4Sy_urlbvnRemKmhFKv-jZp_H7Wr6PkWoE3fojlhBr6LQW23zySX2-vlUsAV-bRAi8uYZnC4aYVRKITtiG62Oc0PrHU7oU_TZhB-DWH3R79op2pb9WArvBAGHoL4tXRDR_xHOcXUreCmPOrSwQnKmmRE3fe1MdeU9zr44rEHY1JOETCsj1kKHzrxOmZk2uhc_5k593ppTRD1UVrOpv3JGknK3Lz0UjLxxGwpe6euAvl3FR5Qc6z1XCz7toapqs46npnmgL1hG-M5bxYOvXiEVHuDNxOAMmhPBfA_RzTV0NMr13w=w687-h915-no" width=500 height=600>
