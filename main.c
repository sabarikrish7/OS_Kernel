#include <stdio.h>
#include <string.h>

static const char *PRIVILEGED[] = {"hlt", "cli", "sti", "in", "out", "lgdt", "lidt", "ltr", "wrmsr", "rdmsr",
                                   "invlpg", "wbinvd", "mov_cr0", "mov_cr3", "mov_cr4"};

static int is_privileged(const char *m) {
    size_t i;
    if (!m) return 0;
    for (i = 0; i < sizeof(PRIVILEGED) / sizeof(PRIVILEGED[0]); i++)
        if (strcmp(PRIVILEGED[i], m) == 0) return 1;
    return 0;
}

/* Apply one event to the CPU. Returns the new ring; sets *fault to 1 if the event faulted. */
static int step(int ring, const char *cmd, const char *arg, int *fault) {
    *fault = 0;
    /* TODO: SYSCALL / INTR enter ring 0, SYSRET / IRET return to ring 3, EXEC of a privileged
       mnemonic faults in ring 3. Keep the ring unchanged for everything else. */
    (void)cmd; (void)arg; (void)is_privileged;
    return ring;
}

int main(void) {
    char line[4096];
    int ring = 3;
    while (fgets(line, sizeof line, stdin)) {
        char *cmd = strtok(line, " \t\r\n");
        char *arg;
        int fault;
        if (!cmd) continue;
        arg = strtok(NULL, " \t\r\n");
        if (strcmp(cmd, "PROBE") == 0) { printf("ring %d\n", ring); continue; }
        ring = step(ring, cmd, arg, &fault);
        if (fault) printf("#GP\n");
    }
    return 0;
}
