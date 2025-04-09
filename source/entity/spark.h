#ifndef SPARK_H
#define SPARK_H
#include "entity.h"

struct _AirWizard;
struct _Screen;

typedef struct _Spark{
	Entity entity;
	int lifeTime;
	double xa, ya;
	double xx, yy;
	int time;
	struct _AirWizard* owner;
} Spark;

void spark_create(Spark* spark, struct _AirWizard* owner, double xa, double ya);
void spark_tick(Spark* spark);
void spark_render(Spark* spark,struct _Screen* screen);

#endif // SPARK_H
