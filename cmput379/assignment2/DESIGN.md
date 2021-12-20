(a) The protocol is based on TCP provided by socket API
    - When an user client or an observer client initialize, they will first send a acknowledgement message to show their type.
    - After acknowleding the type of client, server will establish connection with clients based on their type.

    - Observer will keep sending information to server as long as there exist any changes in the file or directory it is watching,
      each time, observer will send a serialized struct which contains information of inotify events.
    - When server receive serialized data sent by observer, it will deserialize and store it in a data structure, and they will be
      sent to user when it is necessary.

    - After each 'timeinterval' seconds, server will send stored inotify information to user.
    - Server will first send an int to demonstrate the size of the serialized data it will send in a short time.
    - After sedning the first int, server will send serialized data which stores the inotify information from observer i.
    - Above process will be looping until all the inotify information from all observer are sucessfully sent.
    - User will deserialize data and print it to the stdout.

(b) The implemented refinement is the "Server timestamps" functionality, which can be invoked by issuing "make notapp.time".
    Just using make will generate an executable notapp, which is the one without refinement.