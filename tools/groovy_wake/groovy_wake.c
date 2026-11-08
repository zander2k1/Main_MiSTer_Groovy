/*
 * groovy_wake: starts the Groovy core when a PC asks for it.
 *
 * Runs in the background on the MiSTer (started from /media/fat/linux/user-startup.sh). Groovy
 * Dolphin and Groovy OpenMW send a small network message ("GROOVY_START") to UDP port 32110 when
 * the MiSTer doesn't answer on the Groovy port. This program then loads the newest Groovy core in
 * /media/fat/_Utility through the MiSTer main program's command interface (/dev/MiSTer_cmd), the
 * same way MiSTer scripts load cores.
 *
 * It only ever loads the Groovy core and ignores every other message. Requests within 15 seconds
 * of the last one are ignored, so a PC retrying doesn't reload the core while it starts.
 *
 * Build: arm-none-linux-gnueabihf-gcc -O2 -static -o groovy_wake groovy_wake.c
 */

#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define WAKE_PORT 32110
#define WAKE_MESSAGE "GROOVY_START"
#define CORE_DIR "/media/fat/_Utility"
#define CMD_FIFO "/dev/MiSTer_cmd"
#define MIN_INTERVAL_SECONDS 15

/* Newest Groovy core: Groovy_YYYYMMDD.rbf names sort by date. Test builds are skipped. */
static int find_core(char *path, size_t size)
{
	DIR *dir = opendir(CORE_DIR);
	if (!dir) return 0;

	char best[256] = "";
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		const char *name = entry->d_name;
		size_t len = strlen(name);
		if (len >= sizeof(best) || len < 11) continue;
		if (strncasecmp(name, "Groovy_", 7) != 0) continue;
		if (strcasecmp(name + len - 4, ".rbf") != 0) continue;
		if (strstr(name, "_test_") != NULL) continue;
		if (strcmp(name, best) > 0) strcpy(best, name);
	}
	closedir(dir);

	if (!best[0]) return 0;
	snprintf(path, size, "%s/%s", CORE_DIR, best);
	return 1;
}

static int load_core(const char *path)
{
	int fd = open(CMD_FIFO, O_WRONLY | O_NONBLOCK);
	if (fd < 0) return 0;

	char cmd[512];
	int len = snprintf(cmd, sizeof(cmd), "load_core %s\n", path);
	int ok = write(fd, cmd, len) == len;
	close(fd);
	return ok;
}

int main(void)
{
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		perror("groovy_wake: socket");
		return 1;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(WAKE_PORT);
	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("groovy_wake: bind");
		return 1;
	}

	printf("groovy_wake: waiting for requests on UDP port %d\n", WAKE_PORT);
	fflush(stdout);

	time_t last = 0;
	for (;;)
	{
		char msg[64];
		struct sockaddr_in from;
		socklen_t fromlen = sizeof(from);
		ssize_t n = recvfrom(sock, msg, sizeof(msg) - 1, 0, (struct sockaddr *)&from, &fromlen);
		if (n <= 0) continue;
		msg[n] = 0;
		if (strcmp(msg, WAKE_MESSAGE) != 0) continue;

		const char *reply = "BUSY";
		time_t now = time(NULL);
		if (now - last >= MIN_INTERVAL_SECONDS)
		{
			char path[512];
			if (!find_core(path, sizeof(path)))
			{
				reply = "NO_CORE";
			}
			else if (!load_core(path))
			{
				reply = "FAILED";
			}
			else
			{
				reply = "OK";
				last = now;
				printf("groovy_wake: %s asked for the Groovy core, loading %s\n", inet_ntoa(from.sin_addr), path);
				fflush(stdout);
			}
		}
		sendto(sock, reply, strlen(reply), 0, (struct sockaddr *)&from, fromlen);
	}
}
