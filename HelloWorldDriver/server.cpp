#include "shared.h"
#include "sockets.h"
#include "logging.h"

extern uint64_t handle_incoming_packet(const Packet& packet);
extern bool		complete_request(SOCKET client_connection, uint64_t result);

SOCKET create_listener()
{
	SOCKADDR_IN address{};
	
	// tcp, udp
	address.sin_family = AF_INET;

	// 27015
	address.sin_port = htons(server_port);

	int listener = socket_listen(AF_INET, SOCK_STREAM, 0);

	if (listener == -1)
	{
		log("Failed to create listener");
		return INVALID_SOCKET;
	}

	int result;

	result = bind(listener, (SOCKADDR*)&address, sizeof(address));

	if (result == -1)
	{
		log("Failed to bind listener");
		return INVALID_SOCKET;
	}

	result = listen(listener, 1);

	if (result == -1)
	{
		log("Failed to start listener");
		return INVALID_SOCKET;
	}

	return listener;
}

// handle accepted connections
void NTAPI connection_thread(void* connection_socket)
{
	SOCKET connection = SOCKET(ULONG_PTR(connection_socket));

	Packet packet{};

	while (true)
	{
		int result = recv(connection, (void*)&packet, sizeof(packet), 0);
		if (result <= 0)
			break;

		if (result < sizeof(PacketHeader))
			continue;

		if (packet.header.magic != packet_magic)
			continue;

		const auto packet_result = handle_incoming_packet(packet);
		if (!complete_request(connection, packet_result))
			break;
	}
}

void NTAPI server_thread(void*)
{
	// initialise ksocket
	NTSTATUS status = KsInitialize();

	// handle unsuccessful intitialisation
	if (!NT_SUCCESS(status))
	{
		log("Failed to initialize KSOCKET. Status code: %X.", status);
		return;
	}

	SOCKET listener = create_listener();
	
	if (listener == -1)
	{
		log("Failed to create listener");
		PsTerminateSystemThread(STATUS_UNSUCCESSFUL);
		return;
	}

	log("Listening at port %d.", server_port);

	while (true)
	{
		sockaddr	socket_addr{};
		socklen_t	socket_length{};

		int client_sockfd = accept(listener, &socket_addr, &socket_length);

		HANDLE thread_handle = NULL;

		status = PsCreateSystemThread(
			&thread_handle,
			GENERIC_ALL,
			NULL,
			NULL,
			NULL,
			connection_thread,
			(void*)client_sockfd
		);

		/*if (!NT_STATUS(status))
		{
			log("Failed to create thread to handle client connection.");
		}*/

		ZwClose(thread_handle);
	}
}