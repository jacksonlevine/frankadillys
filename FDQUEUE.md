




DONE                                    REALTIME NOISE (POSSIBLE OPTIMIZATION):
                                                get terrain from realtime noise funcs, store changed only


SHELL GROUND:
    Refactor heightmap to have a HeightTile struct {    bool broken          }


Fill all space below heightmap with blocks



Raycast ---> hit block -----> break
|
|
v
hit heightmap
set bool broken


Don't render if broken 