

void sdcard_init_init() {
    // wait 1 ms after power up
    delay_1ms();
    //set SPI frequency between 100khz and 400khz
    spi_low_frequency();
    // deselect (logic 1)
    spi_deselect();
    // Установите сигналы DI и CS в состояние высокого уровня, и выведите 74 или большее количество импульсов тактов на сигнале SCLK
    for(int i = 0; i < 75; i++)
        spi_send(0xFF);
    
}