#ifndef STATUS_H
#define STATUS_H

enum class StatusType{
    none,Stunned,Paralyzed,Frozen,Cursed,Poisoned
};

class Status{
public:
    Status(StatusType type, int duration, int damagePerTurn = 0);

    StatusType getType() const;
    int getDuration() const;
    int getDamagePerTurn() const;
    void decreaseDuration();

private:
    StatusType type;
    int duration;
    int damagePerTurn;
};
#endif // STATUS_H
