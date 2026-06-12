#include "status.h"

Status::Status(StatusType type, int duration, int damagePerTurn)
    : type(type),
    duration(duration),
    damagePerTurn(damagePerTurn)
{}

StatusType Status::getType() const {
    return type;
}

int Status::getDuration() const
{
    return duration;
}

int Status::getDamagePerTurn() const
{
    return damagePerTurn;
}

void Status::decreaseDuration()
{
    duration--;
}
