/**
 * Copyright (c) 2020, 2021, 2022 Adrian Siekierka
 *
 * TinyZoo/GB is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * TinyZoo/GB is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with TinyZoo/GB. If not, see <https://www.gnu.org/licenses/>. 
 *
 * TinyZoo/GB includes code derived from the Reconstruction of ZZT, used under
 * the following license terms:
 *
 * Copyright (c) 2020 Adrian Siekierka
 *
 * Based on a reconstruction of code from ZZT,
 * Copyright 1991 Epic MegaGames, used with permission.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdint.h>

void ElementDefaultTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);
void ElementDamagingTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);
void ElementBoardEdgeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);
void ElementMessageTimerTick(uint8_t stat_id);
void ElementMonitorTick(uint8_t stat_id);
uint8_t ElementPlayerDraw(uint8_t x, uint8_t y);
void ElementPlayerTick(uint8_t stat_id);
void ElementAmmoTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);
void ElementTorchTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);
void ElementGemTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void ElementKeyTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);
void ElementDoorTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void ElementScrollTick(uint8_t stat_id);
void ElementScrollTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void ElementPassageTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

uint8_t ElementDuplicatorDraw(uint8_t x, uint8_t y);
void ElementDuplicatorTick(uint8_t stat_id);

uint8_t ElementBombDraw(uint8_t x, uint8_t y);
void ElementBombTick(uint8_t stat_id);
void ElementBombTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void ElementEnergizerTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

uint8_t ElementStarDraw(uint8_t x, uint8_t y);
void ElementStarTick(uint8_t stat_id);

void ElementBulletTick(uint8_t stat_id);

void ElementWaterTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void ElementForestTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void ElementPushableTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void ElementFakeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void ElementInvisibleTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void ElementBlinkWallTick(uint8_t stat_id);

uint8_t ElementTransporterDraw(uint8_t x, uint8_t y);
void ElementTransporterTick(uint8_t stat_id);
void ElementTransporterTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

uint8_t ElementLineDraw(uint8_t x, uint8_t y);

void ElementBearTick(uint8_t stat_id);

void ElementRuffianTick(uint8_t stat_id);

void ElementSlimeTick(uint8_t stat_id);
void ElementSlimeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void ElementSharkTick(uint8_t stat_id);

uint8_t ElementConveyorCWDraw(uint8_t x, uint8_t y);
void ElementConveyorCWTick(uint8_t stat_id);
uint8_t ElementConveyorCCWDraw(uint8_t x, uint8_t y);
void ElementConveyorCCWTick(uint8_t stat_id);

uint8_t ElementSpinningGunDraw(uint8_t x, uint8_t y);
void ElementSpinningGunTick(uint8_t stat_id);

uint8_t ElementPusherDraw(uint8_t x, uint8_t y);
void ElementPusherTick(uint8_t stat_id);

void ElementLionTick(uint8_t stat_id);
void ElementTigerTick(uint8_t stat_id);

void ElementCentipedeHeadTick(uint8_t stat_id);
void ElementCentipedeSegmentTick(uint8_t stat_id);

uint8_t ElementObjectDraw(uint8_t x, uint8_t y);
void ElementObjectTick(uint8_t stat_id);
void ElementObjectTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);
