#ifndef PROGRESSOBSERVER_H
#define PROGRESSOBSERVER_H

class GoalItem;

class ProgressObserver {
public:
    virtual ~ProgressObserver() = default;
    virtual void onProgressChanged(GoalItem* item) = 0;
};

#endif
