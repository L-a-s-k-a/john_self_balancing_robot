#include "init.h"

void RCC_Init(void)
{
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
    CLEAR_REG(RCC->CFGR);
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);

    SET_BIT(RCC->CR, RCC_CR_HSEON); // Включение внешнего источника тактирования высокой частоты
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET);
    SET_BIT(RCC->CR, RCC_CR_CSSON); // Включение Clock Security

    /*------------------Настройка HCLK на 96 МГц и настройка USB_OTG на 48 МГц------------------*/
    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE); // Источник тактирвоания HSE
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_3);     // Деление источника тактирования на 8 (PLLM)
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_7); // Настройка умножения на 192 (PLLN) (0 1100 0000)
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP);     // Деление частоты после умножения на 2 (PLLP)
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_2);     // Деление частоты после умножения на 4 (PLLQ)

    SET_BIT(RCC->CFGR, RCC_CFGR_SW_PLL);                         // В качестве системного тактирования выбран PLL
    SET_BIT(RCC->CFGR, RCC_CFGR_HPRE_DIV1);                      // Предделитель шины AHB1 настроен на 1 (без деления)
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE1_DIV2);                     // Предделитель шины APB1 настроен на 2 (48МГц)
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE2_DIV1);                     // Предделитель шины APB2 настроен на 1 (96МГц)
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO1);                           // Нстройка вывода частоты PLL на MCO1
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO1PRE_2 | RCC_CFGR_MCO1PRE_1); // Предделитель 4 для вывода на MCO1
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2);                         // Настройка вывода частоты SYSCLK на MCO2
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO2PRE_2 | RCC_CFGR_MCO2PRE_1); // Предделитель 4 для вывода на MCO2

    SET_BIT(FLASH->ACR, FLASH_ACR_LATENCY_5WS); // Установка 5 циклов ожидания для FLASH памяти

    SET_BIT(RCC->CR, RCC_CR_PLLON); // Включение блока PLL
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET);
}

void SysTick_Init(void){
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, (168000 - 1) << SysTick_LOAD_RELOAD_Pos);
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, (168000 - 1) << SysTick_VAL_CURRENT_Pos);
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
}

void GPIO_Init(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN);

    SET_BIT(GPIOC->MODER, GPIO_MODER_MODER0_0);

    SET_BIT(GPIOA->MODER, GPIO_MODER_MODER4_0);
    SET_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED4_1);

    SET_BIT(GPIOA->MODER, GPIO_MODER_MODER7_1);
    SET_BIT(GPIOA->AFR[0], GPIO_AFRL_AFSEL7_1);

    SET_BIT(GPIOA->MODER, GPIO_MODER_MODER6_0);
    SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS6);
    // SET_BIT(GPIOA->AFR[0], GPIO_AFRL_AFSEL6_1);

    /*--------Настройка работы порта PC9 в качестве MCO2--------*/
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODER9_1);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9);
    CLEAR_BIT(GPIOC->AFR[1], GPIO_AFRH_AFSEL9);

    /*--------Настройка работы порта PA8 в качестве MCO1--------*/
    SET_BIT(GPIOA->MODER, GPIO_MODER_MODER8_1);
    SET_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED8);
    CLEAR_BIT(GPIOA->AFR[1], GPIO_AFRH_AFSEL8);
}

void TIM_PWM_Init(void){
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN); //Включение тактирования таймера 3
    TIM3->PSC = 5-1; //48 МГц тактовая частота шины АРВ1, но таймеры тактируются от источника, который всегда умножает на 2
    TIM3->ARR = 4800-1;

    SET_BIT(TIM3->CR1, TIM_CR1_CMS_0);

    /*------------------Режим ШИМ------------------*/
    CLEAR_BIT(TIM3->CCMR1, TIM_CCMR1_CC2S_Msk);
    SET_BIT(TIM3->CCMR1, TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE);
    SET_BIT(TIM3->CCER, TIM_CCER_CC2E);

    // SET_BIT(TIM3->EGR, TIM_EGR_UG);    // Принудительная генерация обновления для загрузки предделителя и ARR
    SET_BIT(TIM3->CR2, TIM_CR2_MMS_1); // Настройка выхода триггера на событие обновления (Update event)
    SET_BIT(TIM3->CR1, TIM_CR1_CEN);   // Включение таймера
    SET_BIT(TIM3->DIER, TIM_DIER_UIE); //Включение Прерывания по переполнению
    NVIC_EnableIRQ(TIM3_IRQn);
}

void ADC_Init(void){
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN);

    //Натсройка канала PA0 для работы в аналоговом режиме
    SET_BIT(GPIOA->MODER, GPIO_MODER_MODER0);
    CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD0);
    SET_BIT(GPIOA->MODER, GPIO_MODER_MODER1);
    CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD1);
    SET_BIT(GPIOA->MODER, GPIO_MODER_MODER2);
    CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD2);

    /* Установка предделителя на 4, АЦП тактируется от шины АРВ2 96 МГц, 
     * а АЦП должно работать на частоте не более 30 МГц, из-за чего
     * следует устанавливать делитель на 4. В резултате частота АЦП будет 24 МГц, 
     * что подходит для стабильной работы.
     */
    SET_BIT(ADC->CCR, ADC_CCR_ADCPRE_0);

    //Включение внутреннего датчика температуры
    SET_BIT(ADC->CCR, ADC_CCR_TSVREFE);

    CLEAR_BIT(ADC1->CR1, ADC_CR1_RES);    //Разрешение АЦП 12 бит (15 циклов частоты АЦП)
    SET_BIT(ADC1->CR1, ADC_CR1_SCAN);     //Выключение сканирования нескольких каналов
    // SET_BIT(ADC1->CR1, ADC_CR1_EOCIE); //Включение прерывания по окончанию преобразования

    CLEAR_BIT(ADC1->CR2, ADC_CR2_ALIGN);  //Выравнивание по правому краю (не отрицательные значения)
    // SET_BIT(ADC1->CR2, ADC_CR2_CONT);  //Включение непрерывного преобразования

    SET_BIT(ADC1->CR2, ADC_CR2_EXTEN_0);  //Включение преобразований по фронту внешнего триггера
    SET_BIT(ADC1->CR2, ADC_CR2_EXTSEL_3); //Выбор в качестве внешнего триггера TIM3 Update event

    SET_BIT(ADC1->CR2, ADC_CR2_DMA);      //Включение DMA
    SET_BIT(ADC1->CR2, ADC_CR2_DDS);      //Режим работы DMA - непрерывный запрос (новой запрос на преобразование не будет отправляться, пока не будет считано предыдущее значение)
    
    SET_BIT(ADC1->SMPR2, ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_2); //56 циклов преобразования для канала 0
    SET_BIT(ADC1->SMPR2, ADC_SMPR2_SMP1_1 | ADC_SMPR2_SMP1_2); //144 циклов преобразования для канала 1 (Из-за высокого сопротивления на входе АЦП)
    SET_BIT(ADC1->SMPR2, ADC_SMPR2_SMP2_0); //15 циклов преобразования для канала 2
    SET_BIT(ADC1->SMPR2, ADC_SMPR2_SMP3_0); //15 циклов преобразования для канала 3

    // Задаём последовательность из 4 каналов (L[3:0] = 3)
    ADC1->SQR1 = (3 << 20);   // L = 3 (всего 4 канала)
    // Заполняем SQ1..SQ4 в регистре SQR3 (каналы 0,1,2,3)
    ADC1->SQR3 = (0 << 0) | (1 << 5) | (2 << 10) | (3 << 15);

    //Включение АЦП
    SET_BIT(ADC1->CR2, ADC_CR2_ADON);
    NVIC_EnableIRQ(ADC_IRQn);

    // Небольшая задержка для стабилизации аналоговой части (рекомендуется)
    for(volatile int i = 0; i < 1000; i++);
}

void DMA_Init(void){
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN);

    // Отключение потока перед настройкой
    DMA2_Stream0->CR &= ~DMA_SxCR_EN;
    while(DMA2_Stream0->CR & DMA_SxCR_EN);

    DMA2_Stream0->NDTR = 4; // Количество данных для передачи (4 в нашем случае)
    DMA2_Stream0->PAR = (uint32_t)(&ADC1->DR);
    DMA2_Stream0->M0AR = (uint32_t)adc_buf; // Адрес буфера в памяти для хранения данных

    /* Конфигурация:
    *   - канал 0 (для ADC1)
    *   - направление: периферия → память (DIR=00)
    *   - размер данных: 16 бит (PSIZE=01, MSIZE=01)
    *   - инкремент памяти (MINC=1)
    *   - циклический режим (CIRC=1) – автоматический перезапуск
    *   - прерывание по завершении передачи (TCIE=1)
    *   - приоритет средний (PL=01)
    */
    DMA2_Stream0->CR = (0 << DMA_SxCR_CHSEL_Pos)   // канал 0
                     | (1 << DMA_SxCR_PSIZE_Pos)   // PSIZE = 01 (16 бит)
                     | (1 << DMA_SxCR_MSIZE_Pos)   // MSIZE = 01 (16 бит)
                     | (0 << DMA_SxCR_DIR_Pos)     // DIR = 00 (периферия → память)
                     | DMA_SxCR_MINC               // Увеличение адреса памяти после каждого успешного обмена
                     | DMA_SxCR_CIRC               // Циклический режим
                     | DMA_SxCR_TCIE               // Включение прерывания по окончанию передачи
                     | (1 << DMA_SxCR_PL_Pos);     // PL[0]=1, PL[1]=0 → средний приоритет

    // Включение потока
    DMA2_Stream0->CR |= DMA_SxCR_EN;

    // Настройка NVIC для прерывания DMA2 Stream0
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    NVIC_SetPriority(DMA2_Stream0_IRQn, 1);
}

void I2C_Init(void){
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    GPIOB->MODER |= GPIO_MODER_MODER6_1 | GPIO_MODER_MODER9_1;
    GPIOB->OTYPER |= GPIO_OTYPER_OT6 | GPIO_OTYPER_OT9;
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED9;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD9_0;
    GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_2; 
    GPIOB->AFR[1] |= GPIO_AFRH_AFSEL9_2;

    I2C1->CR2 |= I2C_CR2_FREQ_4 | I2C_CR2_FREQ_5;
    I2C1->CCR |= 240; // Для Standard Mode (100 кГц) при 48 МГц
    I2C1->TRISE |= 49; // 1000 нс ((1000 * 48) / 1000) + 1 = 49 для Standard Mode (100 кГц)
    // I2C1->CCR |= I2C_CCR_FS | 40; // Для Fast Mode (400 кГц) при 48 МГц
    // I2C1->TRISE |= 15; // Для Fast Mode(400 кГц) 300 нс ((300 * 48) / 1000) + 1 = 14.4, округляем до 15 
    I2C1->CR1 |= I2C_CR1_PE; // Включение I2C1
}