#include "work.h"


#define PRT_LOG(X...)	sysprt->alog(X)	
//#define PRT_LOG(X...)	printf(X)	

/*----------------------------------------------------------------------------------*/

static uint32_t boot_count = 0;
static time_t boot_time[10] = {0, 1, 2, 3};

/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/

/* default KV nodes */
static struct fdb_default_kv_node default_kv_table[] = {
        {"username", "synwit", 0}, /* string KV */
        {"password", "123456", 0}, /* string KV */
        {"boot_count", &boot_count, sizeof(boot_count)}, /* int type KV */
        {"boot_time", &boot_time, sizeof(boot_time)},    /* int array type KV */
//				{"total_pul",},	/*总电能*/
};
/* KVDB object */
static struct fdb_kvdb kvdb_onchip = { 0 };

fdb_kvdb_t syskvdb = &kvdb_onchip;

//static struct fdb_kvdb kvdb_spi_flash = { 0 };
/* TSDB object */
struct fdb_tsdb tsdb = { 0 };
/* counts for simulated timestamp */
static int counts = 0;

extern void kvdb_basic_sample(fdb_kvdb_t kvdb);
extern void kvdb_type_string_sample(fdb_kvdb_t kvdb);
extern void kvdb_type_blob_sample(fdb_kvdb_t kvdb);
extern void tsdb_sample(fdb_tsdb_t tsdb);

static void lock(fdb_db_t db)
{
    __disable_irq();
}

static void unlock(fdb_db_t db)
{
    __enable_irq();
}

//static fdb_time_t get_time(void)
//{
//    /* Using the counts instead of timestamp.
//     * Please change this function to return RTC time.
//     */
//    return ++counts;
//}

//flashDB上电初始化
int flashdb_boot_init(void)
{
    fdb_err_t result;
    
#ifdef FDB_USING_KVDB
    { /* KVDB Sample */
        struct fdb_default_kv default_kv;

        default_kv.kvs = default_kv_table;
        default_kv.num = sizeof(default_kv_table) / sizeof(default_kv_table[0]);
        
        printf("\r\n ================================= fdb_kvdb1 ===================================== \r\n\r\n");
        /* set the lock and unlock function if you want */
        fdb_kvdb_control(&kvdb_onchip, FDB_KVDB_CTRL_SET_LOCK, (void *)lock);
        fdb_kvdb_control(&kvdb_onchip, FDB_KVDB_CTRL_SET_UNLOCK, (void *)unlock);
        
        /* Key-Value database initialization
         *
         *       &kvdb: database object
         *       "env": database name
         * "fdb_kvdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         * &default_kv: The default KV nodes. It will auto add to KVDB when first initialize successfully.
         *        NULL: The user data if you need, now is empty.
         */
        result = fdb_kvdb_init(&kvdb_onchip, "env", "fdb_kvdb1", &default_kv, NULL);

        if (result != FDB_NO_ERR) {
            return -1;
        }

//        /* run basic KV samples */
//        kvdb_basic_sample(&kvdb_onchip);
//        /* run string KV samples */
//        kvdb_type_string_sample(&kvdb_onchip);
//        /* run blob KV samples */
//        kvdb_type_blob_sample(&kvdb_onchip);
        
    }
#endif /* FDB_USING_KVDB */

#ifdef FDB_USING_TSDB
    { /* TSDB Sample */
        /* set the lock and unlock function if you want */
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_LOCK, (void *)lock);
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_UNLOCK, (void *)unlock);
        /* Time series database initialization
         *
         *       &tsdb: database object
         *       "log": database name
         * "fdb_tsdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         *    get_time: The get current timestamp function.
         *         128: maximum length of each log
         *        NULL: The user data if you need, now is empty.
         */
        result = fdb_tsdb_init(&tsdb, "log", "fdb_tsdb1", get_time, 128, NULL);
        /* read last saved time for simulated timestamp */
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_GET_LAST_TIME, &counts);

        if (result != FDB_NO_ERR) {
            return -1;
        }

        /* run TSDB sample */
        tsdb_sample(&tsdb);
    }
#endif /* FDB_USING_TSDB */
    
    return 0;
}

