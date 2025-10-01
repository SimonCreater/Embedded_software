void Task(void* pdata)
{
    OS_RDY_GRP OSRdyGrp;       /* Ready list group                         */
    INT8U OSRdyTbl[8];         /* Table of tasks which are ready to run    */

    INT8U x;
    INT8U y;
    unsigned char a, b;
    INT8U err;
    INT8U number[4];
    int i;
    int count = 0;
    INT8U final = 64;
    INT8U temp = 64;

    for (;;) {
        OSRdyGrp = 0;
        memset(OSRdyTbl, 0, sizeof(INT8U) * 8);

        OSSemPend(RandomSem, 0, &err);
        number[0] = random(64);
        number[1] = random(64);
        number[2] = random(64);
        number[3] = random(64);
        OSSemPost(RandomSem);

        for (i = 0; i < 4; i++)
        {
            OSRdyGrp |= OSMapTbl[number[i] >> 3];
            OSRdyTbl[number[i] >> 3] |= OSMapTbl[number[i] & 0x07];
        }

        temp = (OSUnMapTbl[OSRdyGrp] << 3) + OSUnMapTbl[OSRdyTbl[OSUnMapTbl[OSRdyGrp]]];

        if (temp < final && number[0] != NULL)
        {
            final = temp;

            for (x = 0; x < 80; x++)
            {
                for (y = 0; y < 16; y++)
                {
                    if ((count % 4) == 0) { PC_DispChar(x, y + 5, NULL, DISP_BGND_RED); }
                    else if ((count % 4) == 1) { PC_DispChar(x, y + 5, NULL, DISP_BGND_BLUE); }
                    else if ((count % 4) == 2) { PC_DispChar(x, y + 5, NULL, DISP_BGND_BROWN); }
                    else if ((count % 4) == 3) { PC_DispChar(x, y + 5, NULL, DISP_BGND_GREEN); }
                }
            }
            count++;
            temp = 64;
        }
        OSTimeDly(200);
    }
    if (final == 0)
    {
        final = 64;
    }
}