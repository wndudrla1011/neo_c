#ifndef UPDATE_TABLE_H
#define UPDATE_TABLE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../db.h"
#include "../table.h"
#include "../domain.h"
#include "../data.h"

#define MAX_COLUMN 20 // 최대 속성 값 개수
#define MAX_INPUT 100 // 최대 입력 값 길이

void query_update(DB *db, Table *table, Domain *domain, Data *data)
{
    if (table->cadinality == 0) // insert 0회
    {
        printf("Empty set\n");
        return;
    }

    
}

#endif