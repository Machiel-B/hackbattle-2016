cdef extern from "deezer-player.h":
pass

cdef extern from "deezer-connect.h":
    unsigned int dz_index_in_playlist
    enum dz_tracklist_autoplay_mode_t