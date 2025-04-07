CC = gcc
CFLAGS = -Wall
TARGETS = catcher sender

all: $(TARGETS)

catcher: catcher.c
	$(CC) $(CFLAGS) -o catcher catcher.c

sender: sender.c
	$(CC) $(CFLAGS) -o sender sender.c

run-test: all
	@echo "=== TEST: Uruchamianie catchera ==="
	./catcher > catcher_output.txt & \
	CATCHER_PID=$$!; \
	sleep 1; \
	ACTUAL_PID=`head -n 1 catcher_output.txt | awk '{print $$2}'`; \
	echo "CATCHER PID: $$ACTUAL_PID"; \
	echo "=== TEST: Wysyłanie trybu 1 ==="; \
	./sender $$ACTUAL_PID 1; \
	sleep 1; \
	echo "=== TEST: Wysyłanie trybu 2 ==="; \
	./sender $$ACTUAL_PID 2; \
	sleep 3; \
	echo "=== TEST: Wysyłanie trybu 4 (Ctrl+C reakcja) ==="; \
	./sender $$ACTUAL_PID 4; \
	sleep 1; \
	echo "=== TEST: Wysyłanie trybu 5 (zakończenie) ==="; \
	./sender $$ACTUAL_PID 5; \
	wait $$CATCHER_PID; \
	echo "=== TEST ZAKOŃCZONY ==="

clean:
	rm -f $(TARGETS) catcher_output.txt
