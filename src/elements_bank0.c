#include "elements.h"
#include "elements_utils.h"
#include "gamevars.h"
#include "game.h"
#include "input.h"
#include "math.h"
#include "message_consts.h"
#include "oop.h"
#include "sound_consts.h"
#include "timer.h"

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

#include "element_defs.inc"
