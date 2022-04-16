#pragma once
#include "Drawable.h"
#include "GameEntity.h"
#include "Sabotageable.h"

class SabotageItem: public Drawable, public GameEntity, public Sabotageable{

public:
	SabotageItem();
	~SabotageItem(void);
};
