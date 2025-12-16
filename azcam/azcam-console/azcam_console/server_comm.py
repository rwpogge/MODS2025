"""
Class to communicate with azcamserver.
"""

import azcam
import azcam.sockets
import azcam.exceptions


class ServerCommunication(object):
    """
    Server connection tool for consoles.
    Usually implemented as the "server" tool.
    """

    def __init__(self) -> None:
        self.remserver = azcam.sockets.SocketInterface()
        self.connected = False

    def connect(self, host="localhost", port=2402):
        """
        Connect to azcamserver.
        """

        self.host = host
        self.port = port

        self.remserver.host = host
        self.remserver.port = port

        if self.remserver.open():
            connected = True
            self.remserver.command("register console")
        else:
            connected = False

        self.connected = connected

        return connected

    def command(self, command):
        """
        Send a command to a server process using the 'server' object in the database.
        This command traps all errors and returns exceptions and as error string.

        Returns None or a string.
        """

        # get tokenized reply - check for comm error
        try:
            reply = self.remserver.command(command)
        except azcam.exceptions.AzcamError as e:
            if e.error_code == 2:
                raise
                # raise azcam.exceptions.AzcamError("could not connect to server")
            else:
                raise

        if command in ["exposure.get_status"]:
            return reply[0][3:]

        # status for socket communications is OK or ERROR
        if reply[0] == "ERROR":
            raise azcam.exceptions.AzcamError(f"command error: {reply}")
        elif reply[0] == "OK":
            if len(reply) == 1:
                return None
            elif len(reply) == 2:
                return reply[1]
            else:
                return reply[1:]
        else:
            raise azcam.exceptions.AzcamError(
                f"invalid server response: { ' '.join(reply)}"
            )

        return  # can't get here
