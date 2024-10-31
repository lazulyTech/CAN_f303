//
// Created by Eisuke Kato on 24/10/22.
//

#ifndef CAN_H
#define CAN_H
#include <string.h>

#include "stm32f3xx_hal_can.h"

// 送受信用CANハンドラー
CAN_HandleTypeDef *_hcan;
// 受信ID
uint32_t _can_id[2] = {0};
void (*_can_callback)(uint32_t id, uint8_t* data, uint8_t dlc);

typedef union {
    uint8_t data8[8];
    uint16_t data16[4];
    uint32_t data32[2];
} _can_data_t;

CAN_TxHeaderTypeDef _can_TxHeader;
_can_data_t _can_tx_data = {0};

_can_data_t _can_rx_buf_id1 = {0};
_can_data_t _can_rx_buf_id2 = {0};

/**
 * 受信割り込み関数
 * @param hcan CAN_HandleTypeDef
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) {
        Error_Handler();
    } else {
        uint32_t id = RxHeader.ExtId;
        // if (id == _can_id[0]) {
        //     memcpy(_can_rx_buf_id1.data8, RxData, RxHeader.DLC);
        //     for (uint8_t i = 0; i < 10; i++) {
        //         HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        //         HAL_Delay(50);
        //         HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        //         HAL_Delay(50);
        //     }
        // }
        // else if (id == _can_id[1]) {
        //     memcpy(_can_rx_buf_id2.data8, RxData, RxHeader.DLC);
        //     for (uint8_t i = 0; i < 5; i++) {
        //         HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        //         HAL_Delay(100);
        //         HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        //         HAL_Delay(100);
        //     }
        // }

        if (_can_callback != NULL) {
            _can_callback(id, RxData, RxHeader.DLC);
        }
        // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        // printf("id=%d [0]=%d [1]=%d [2]=%d\r\n",RxHeader.ExtId,RxData[0],RxData[1],RxData[2]);
        // printf("id=%d: ", RxHeader.ExtId);
        // for (uint8_t i = 0; i < 8; i++) {
        //   // printf("%3d, ", i, RxData[i]);
        // }
        // printf("\r\n");
    }
}

/**
 * CANの初期化関数
 * @param hcan CAN_HandleTypeDef
 */
void can_init(CAN_HandleTypeDef *hcan) {
    _hcan = hcan;
    _can_TxHeader.ExtId = 0;
    _can_TxHeader.RTR = CAN_RTR_DATA;
    _can_TxHeader.IDE = CAN_ID_EXT;
    _can_TxHeader.DLC = 8;
    _can_TxHeader.TransmitGlobalTime = DISABLE;
}

/**
 * Filterを設定
 * @param FilterID1 1つ目のID
 * @param Mask1 0で無効、1で有効
 * @param FilterID2 2つ目のID
 * @param Mask2 0で無効、1で有効
 */
void can_configFilter_mask(uint32_t FilterID1, uint32_t Mask1, uint32_t FilterID2, uint32_t Mask2) {
    _can_id[0] = FilterID1;
    _can_id[1] = FilterID2;

    CAN_FilterTypeDef filter;
    filter.FilterIdHigh         = FilterID1 << 5;
    filter.FilterIdLow          = FilterID2 << 5;
    filter.FilterMaskIdHigh     = Mask1 << 5;
    filter.FilterMaskIdLow      = Mask2 << 5;
    // filter.FilterMaskIdHigh     = (Mask1 << 5) | 0x8;
    // filter.FilterMaskIdLow      = (Mask2 << 5) | 0x8;
    filter.FilterScale          = CAN_FILTERSCALE_16BIT;
    filter.FilterFIFOAssignment = CAN_RX_FIFO0;
    filter.FilterBank           = 0;
    filter.FilterMode           = CAN_FILTERMODE_IDLIST;
    filter.SlaveStartFilterBank = 0;
    filter.FilterActivation = ENABLE;

    if (HAL_CAN_ConfigFilter(_hcan, &filter) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * 受信コールバック関数を設定
 * @param Callback
 */
void can_setReceiveCallback(void* Callback) {
    _can_callback = Callback;
}

/**
 * CAN通信を開始
 */
void can_start() {
    if (HAL_CAN_Start(_hcan) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_CAN_ActivateNotification(_hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * データ送信
 * @param id CANID
 * @param data
 * @param dlc DLC
 */
int can_transmitData(const uint32_t id, uint8_t *data, uint16_t dlc) {
    uint32_t TxMailbox;
    _can_TxHeader.ExtId = id;
    _can_TxHeader.DLC = dlc;
    *_can_tx_data.data8 = data;
    memcpy(_can_tx_data.data8, data, _can_TxHeader.DLC);
    if (HAL_CAN_AddTxMessage(_hcan, &_can_TxHeader, _can_tx_data.data8, &TxMailbox) != HAL_OK) {
        Error_Handler();
        return 1;
    }
    while (HAL_CAN_GetTxMailboxesFreeLevel(_hcan) != 3) {}
    return 0;
}

#endif //CAN_H
