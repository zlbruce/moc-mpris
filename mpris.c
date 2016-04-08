/*
 * =====================================================================================
 *
 *       Filename:  mpris.c
 *
 *    Description:  mpris
 *
 *        Version:  1.0
 *        Created:  2013年06月04日 15时29分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhanglei (zhanglei), zhangleibruce@gmail.com
 *        Company:  zlb.me
 *
 * =====================================================================================
 */

#include <gio/gio.h>
#include "server.h"
#include "audio.h"
#include "protocol.h"
#include "mpris_gc.h"

#define MPRIS_BUS_NAME "org.mpris.MediaPlayer2.moc"
#define MPRIS_OBJECT   "/org/mpris/MediaPlayer2"


static OrgMprisMediaPlayer2 *mpris_media_interface = NULL;
static OrgMprisMediaPlayer2Player *mpris_player_interface = NULL;

static gboolean on_handle_quit (
    OrgMprisMediaPlayer2 *object,
    GDBusMethodInvocation *invocation)
{
    set_server_quit();

    org_mpris_media_player2_complete_raise(object, invocation);

    return TRUE;
}

static gboolean on_handle_next (
    OrgMprisMediaPlayer2Player *object,
    GDBusMethodInvocation *invocation)
{
    audio_next();

    org_mpris_media_player2_player_complete_next(object, invocation);

    return TRUE;
}

static gboolean on_handle_previous (
    OrgMprisMediaPlayer2Player *object,
    GDBusMethodInvocation *invocation)
{
    audio_prev();

    org_mpris_media_player2_player_complete_previous(object, invocation);

    return TRUE;
}

static gboolean on_handle_pause (
    OrgMprisMediaPlayer2Player *object,
    GDBusMethodInvocation *invocation)
{
    audio_pause();

    org_mpris_media_player2_player_complete_pause(object, invocation);

    return TRUE;
}

static gboolean on_handle_playpause(
    OrgMprisMediaPlayer2Player *object,
    GDBusMethodInvocation *invocation)
{
    int state = audio_get_state();
    if(state == STATE_PAUSE)
    {
        audio_unpause();
    }
    else if(state == STATE_PLAY)
    {
        audio_pause();
    }

    org_mpris_media_player2_player_complete_play_pause(object, invocation);

    return TRUE;
}

static gboolean on_handle_stop(
    OrgMprisMediaPlayer2Player *object,
    GDBusMethodInvocation *invocation)
{
    audio_stop();

    org_mpris_media_player2_player_complete_stop(object, invocation);

    return TRUE;
}

static gboolean on_handle_play(
    OrgMprisMediaPlayer2Player *object,
    GDBusMethodInvocation *invocation)
{
    audio_unpause();

    org_mpris_media_player2_player_complete_play(object, invocation);

    return TRUE;
}
static const char* uri_schemes[] = {
    "file",
    NULL
};

static const char* mime_type [] = {
    "mp3",
    NULL
};
static void on_bus_acquired (GDBusConnection *connection,
        const gchar     *name,
        gpointer         user_data)
{
    /* This is where we'd export some objects on the bus */
    mpris_media_interface  = org_mpris_media_player2_skeleton_new ();
    g_signal_connect (mpris_media_interface,
            "handle-quit",
            G_CALLBACK (on_handle_quit),
            NULL);
    org_mpris_media_player2_set_can_quit(mpris_media_interface, TRUE);
    org_mpris_media_player2_set_can_raise(mpris_media_interface, FALSE);
    org_mpris_media_player2_set_has_track_list(mpris_media_interface, FALSE);
    org_mpris_media_player2_set_identity(mpris_media_interface, "MOC");
    org_mpris_media_player2_set_desktop_entry(mpris_media_interface, "moc");
    org_mpris_media_player2_set_supported_uri_schemes(mpris_media_interface, uri_schemes);
    org_mpris_media_player2_set_supported_mime_types(mpris_media_interface, mime_type);
    //org_mpris_media_player2_set_desktop_entry (OrgMprisMediaPlayer2 *object, const gchar *value);

    mpris_player_interface = org_mpris_media_player2_player_skeleton_new ();
    g_signal_connect (mpris_player_interface,
            "handle-next",
            G_CALLBACK (on_handle_next),
            NULL);
    g_signal_connect (mpris_player_interface,
            "handle-previous",
            G_CALLBACK (on_handle_previous),
            NULL);
    g_signal_connect (mpris_player_interface,
            "handle-pause",
            G_CALLBACK (on_handle_pause),
            NULL);
    g_signal_connect (mpris_player_interface,
            "handle-play-pause",
            G_CALLBACK (on_handle_playpause),
            NULL);
    g_signal_connect (mpris_player_interface,
            "handle-stop",
            G_CALLBACK (on_handle_stop),
            NULL);
    g_signal_connect (mpris_player_interface,
            "handle-play",
            G_CALLBACK (on_handle_play),
            NULL);
    org_mpris_media_player2_player_set_can_go_next(mpris_player_interface, TRUE);
    org_mpris_media_player2_player_set_can_go_previous(mpris_player_interface, TRUE);
    org_mpris_media_player2_player_set_can_play(mpris_player_interface, TRUE);
    org_mpris_media_player2_player_set_can_pause(mpris_player_interface, TRUE);
    org_mpris_media_player2_player_set_can_control(mpris_player_interface, TRUE);
    GError *error = NULL;
    if( !g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (mpris_media_interface),
            connection,
            MPRIS_OBJECT,
            &error))
    {
        g_warning("g_dbus_interface_skeleton_export failed: %s", error->message);
    }

    if( !g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (mpris_player_interface),
            connection,
            MPRIS_OBJECT,
            &error))
    {
        g_warning("g_dbus_interface_skeleton_export failed: %s", error->message);
    }
    
}

static void on_name_acquired (GDBusConnection *connection,
        const gchar     *name,
        gpointer         user_data)
{
    g_print ("Acquired the name %s on the session bus\n", name);
}

static void on_name_lost (GDBusConnection *connection,
        const gchar     *name,
        gpointer         user_data)
{
    g_print ("Lost the name %s on the session bus\n", name);
}

int mpris_init()
{
    g_type_init();
    /*guint owner_id;

    owner_id = */g_bus_own_name (G_BUS_TYPE_SESSION,
            MPRIS_BUS_NAME,
            G_BUS_NAME_OWNER_FLAGS_NONE,
            on_bus_acquired,
            on_name_acquired,
            on_name_lost,
            NULL,
            NULL);

    //loop = g_main_loop_new (NULL, FALSE);
    //g_main_loop_run (loop);

    //g_bus_unown_name (owner_id);
    return 0;
}

void mpris_handle_event()
{
    GMainContext * main_context = g_main_context_default();
    while(g_main_context_iteration(main_context, FALSE));
}

void mpris_quit()
{
}
