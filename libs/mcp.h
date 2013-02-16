typedef void (*init_fun)(void);
typedef void (*deinit_fun)(void);
typedef uint8_t (*tick_fun)(void);

void registerAnimation(char *, init_fun init,tick_fun tick, deinit_fun deinit,uint16_t t, uint16_t count);
void mcp_tick(void);
