#include "EchoBotModule.h"

#include "mesh/MeshService.h"
#include "mesh/Router.h"
#include "mesh/NodeDB.h"

#include <esp_system.h>

#include <cstring>
#include <cctype>
#include <ctime>

#define CHAN 2

extern MeshService *service;
extern Router *router;
extern NodeDB *nodeDB;

QueueHandle_t EchoBotModule::botQueue = nullptr;

static const char *fortune_messages[] = {
    "La vraie connaissance consiste à reconnaître l’étendue de son ignorance.",
    "Le bois pourri ne peut être sculpté.",
    "Les hommes ont une nature semblable ; ce sont leurs habitudes qui les éloignent.",
    "Notre plus grande gloire n’est pas de ne jamais tomber, mais de nous relever à chaque chute.",
    "Peu importe la lenteur du pas, tant que l’on ne s’arrête pas.",
    "Celui qui déplace une montagne commence par enlever de petites pierres.",
    "Apprendre sans réfléchir est vain ; réfléchir sans apprendre est dangereux.",
    "L’homme supérieur est exigeant envers lui-même ; l’homme ordinaire envers les autres.",
    "Celui qui sait ne parle pas beaucoup ; celui qui parle beaucoup ne sait pas.",
    "Corriger une erreur est déjà un progrès.",
    "Celui qui gouverne par la vertu ressemble à l’étoile polaire : immobile, tout tourne autour d’elle.",
    "Celui qui sait se contenter est riche.",
    "Un voyage de mille pas commence sous vos pieds.",
    "Trop de mots épuisent la vérité.",
    "Celui qui se plie ne se brise pas.",
    "Plus on agit, moins on obtient.",
    "Le doux triomphe du dur.",
    "Qui s’élève sera abaissé.",
    "Le vide rend l’usage possible.",
    "Le poisson oublie l’eau ; l’homme oublie l’ordre naturel des choses.",
    "Celui qui s’attache perd.",
    "Le sage suit les choses sans les forcer.",
    "Quand l’esprit est libre, le corps suit.",
    "La vraie liberté ne se remarque pas.",
    "Le ciel et la terre ne jugent pas.",
    "Un voyage commence toujours par un premier pas.",
    "La patience est une force silencieuse.",
    "Ce que vous cherchez vous cherche aussi.",
    "Un changement inattendu vous apportera une clarté nouvelle.",
    "L’excès éloigne de l’équilibre.",
    "Ce qui est naturel ne force rien.",
    "Le calme précède la justesse.",
    "Vouloir trop empêche d’obtenir.",
    "La simplicité révèle l’essentiel.",
    "Un pas modeste aujourd’hui facilitera un grand pas demain.",
    "La patience révèle ce que la précipitation cache.",
    "Un détour vous évitera une perte inutile.",
    "Ce que vous construisez lentement tiendra longtemps.",
    "Un silence bien choisi éclairera la situation.",
    "Votre constance porte déjà ses effets.",
    "Un choix simple dissipera une confusion persistante.",
    "Ce qui arrive trop vite repart souvent trop tôt.",
    "Une décision calme vaut mieux qu’une réaction immédiate.",
    "Vous n’êtes pas en retard, vous êtes en chemin.",
    "Le bon moment se reconnaît à sa tranquillité.",
    "Tout n’a pas besoin d’être décidé aujourd’hui.",
    "Ce qui mûrit lentement nourrit plus sûrement.",
    "Le temps révèle les vraies priorités.",
    "Attendre n’est pas toujours perdre.",
    "Un refus clair protège votre énergie.",
    "Choisir moins, c’est souvent choisir mieux.",
    "Une option écartée simplifie le reste.",
    "Votre choix n’a pas besoin d’être justifié.",
    "Ce qui est juste s’impose sans effort.",
    "Votre attention transforme votre expérience.",
    "La clarté commence par l’acceptation.",
    "Un esprit calme voit plus loin.",
    "La simplicité apaise durablement.",
    "Votre intuition mérite d’être écoutée.",
    "Chaque pas compte, même discret.",
    "Le chemin se dessine en avançant.",
    "Un détour enrichit parfois le voyage.",
    "La progression importe plus que la vitesse.",
    "Vous avancez plus que vous ne le croyez.",
    "Ce qui semble insignifiant aujourd’hui prendra du sens plus tard.",
    "Un effort régulier dépasse un élan isolé.",
    "La stabilité naît de petits ajustements.",
    "Un regard posé clarifie les priorités.",
    "Ce que vous laissez aller vous libère.",
    "La réponse douce apaise la colère.",
    "L’orgueil précède la chute.",
    "La patience vaut mieux que la force.",
    "Trop de paroles mènent au vide.",
    "Le cœur joyeux est une bonne médecine.",
    "Mieux vaut peu avec la paix que beaucoup avec le trouble.",
    "Celui qui se hâte trébuche.",
    "La sagesse aime la discrétion.",
    "Qui veille sur ses paroles protège sa vie.",
    "La mesure rend la chose durable.",
    "Il y a un temps pour chaque chose.",
    "La fin vaut mieux que le commencement.",
    "Ce qui est tordu ne se redresse pas par la force.",
    "La sagesse éclaire le chemin.",
    "L’écoute précède la compréhension.",
    "Celui qui observe apprend.",
    "Le calme précède la clarté.",
    "L’excès appauvrit même l’abondance.",
    "La douceur désarme plus que la violence.",
    "Le silence protège parfois mieux que la parole.",
    "Celui qui se connaît évite bien des fautes.",
    "L’impatience gâche le fruit du travail.",
    "La réflexion affermit les projets.",
    "Un ami fidèle est un appui sûr.",
    "La sagesse se reconnaît dans la retenue.",
    "La hâte promet plus qu’elle ne tient.",
    "Le regard tranquille voit plus loin.",
    "La constance l’emporte sur l’élan.",
    "Celui qui apprend corrige sa route.",
    "La prudence devance le danger.",
    "Le trouble brouille le jugement.",
    "Une parole juste vaut mieux qu’un long discours.",
    "L’expérience enseigne ce que les mots ignorent.",
    "La paix intérieure rend riche.",
    "Celui qui se compare s’égare.",
    "La sagesse préfère la simplicité.",
    "La colère aveugle le discernement.",
    "La lenteur protège des erreurs.",
    "Le discernement naît de l’écoute.",
    "Celui qui s’élève trop haut perd l’équilibre.",
    "La retenue donne de la force aux actes.",
    "Le juste milieu garde de l’excès.",
    "Le calme révèle l’essentiel.",
    "La modération éclaire le choix.",
    "L’agitation fatigue l’esprit.",
    "Le regard posé éclaire la décision.",
    "La constance construit en silence.",
    "L’excès de confiance précède la faute.",
    "Le repos rend la pensée plus juste.",
    "La sagesse ne se presse pas.",
    "Celui qui parle peu observe mieux.",
    "La clarté vient avec la patience.",
    "L’attention évite bien des détours.",
    "Le jugement hâtif se trompe souvent.",
    "La mesure protège la durée.",
    "L’esprit calme voit plus large.",
    "La retenue garde la liberté.",
    "L’écoute sincère rapproche.",
    "La simplicité allège le chemin.",
    "Celui qui apprend chaque jour reste jeune.",
    "La réflexion précède l’action juste.",
    "Le silence éclaire parfois plus que les mots.",
    "La sagesse fuit l’agitation.",
    "La lenteur soigne la précipitation.",
    "Le discernement évite l’excès.",
    "La patience révèle la voie.",
    "Le calme protège la justesse.",
    "L’équilibre rend solide.",
    "La sobriété rend libre.",
    "La constance vaut mieux que la force.",
    "Celui qui écoute progresse.",
    "Le repos restaure la clarté.",
    "La retenue garde l’essentiel.",
    "Le trop-plein trouble l’esprit.",
    "La sagesse aime le temps long.",
    "La parole mesurée porte plus loin.",
    "Le regard intérieur éclaire la route.",
    "La patience prépare la paix.",
    "L’attention révèle le détail utile.",
    "Le discernement naît du silence.",
    "La modération protège la joie.",
    "La réflexion évite le regret.",
    "Le calme rend le jugement plus sûr.",
    "La sagesse ne cherche pas à briller.",
    "La lenteur évite la chute.",
    "La mesure rend durable.",
    "Le silence apaise le trouble.",
    "La constance éclaire l’avenir.",
    "La simplicité révèle la vérité.",
    "Celui qui se maîtrise garde sa force.",
    "L’écoute attentive précède la réponse juste.",
    "La sagesse préfère la profondeur au bruit.",
    "Le calme nourrit la compréhension.",
    "La retenue protège la clarté.",
    "Le temps révèle ce que la hâte cache.",
    "La modération guide le pas sûr.",
    "La constance dépasse l’élan.",
    "La paix intérieure éclaire toute chose.",
    "Ce qui est vu clairement perd son pouvoir de trouble.",
    "Nommer une chose la rend plus légère.",
    "La réalité ne se presse jamais.",
    "Ce qui est accepté cesse de lutter.",
    "Voir juste suffit souvent à agir juste.",
    "Une limite claire protège la liberté.",
    "Renoncer à l’inutile rend disponible.",
    "Ce qui déborde se perd.",
    "La mesure préserve l’élan.",
    "S’arrêter fait partie du chemin.",
    "Agir juste demande parfois de ne rien ajouter.",
    "L’effort inutile fatigue le résultat.",
    "Ce qui est forcé résiste.",
    "L’action juste ne laisse pas de trace.",
    "Faire moins permet parfois de faire mieux.",
    "Ce qui trouble révèle souvent une attente.",
    "L’inconfort indique un ajustement possible.",
    "Se comprendre allège le monde.",
    "Le regard posé transforme l’expérience.",
    "L’attention calme précède la compréhension.",
    "Ce qui dure ne se montre pas vite.",
    "Le temps polit ce que la force brise.",
    "Ce qui revient mérite d’être regardé.",
    "L’essentiel persiste sans bruit.",
    "Ce qui doit venir n’a pas besoin d’être tiré.",
    "Ce à quoi vous résistez persiste.",
    "Ce que vous observez sans juger se transforme.",
    "La souffrance naît souvent de l’attachement.",
    "L’impermanence rend chaque instant précieux.",
    "Ce qui passe ne mérite pas d’être saisi.",
    "La paix commence quand l’attente cesse.",
    "L’esprit agité fabrique ses propres obstacles.",
    "Voir clairement est déjà agir.",
    "Ce qui change ne peut être possédé.",
    "Le présent suffit quand on y demeure.",
    "L’acceptation n’est pas une défaite.",
    "Le désir excessif trouble la vision.",
    "Ce qui est accueilli s’apaise.",
    "La liberté commence avec le non-attachement.",
    "Un esprit clair allège le corps.",
    "Ce qui est vu comme transitoire fait moins mal.",
    "La simplicité ouvre l’espace intérieur.",
    "L’attention dissout bien des tensions.",
    "Le silence révèle ce qui demeure.",
    "L’effort juste est sans crispation.",
    "Ce qui est vécu pleinement s’achève paisiblement.",
    "L’équilibre naît entre effort et lâcher-prise.",
    "La clarté vient quand l’on cesse de vouloir maîtriser.",
    "Ce qui est observé perd de son emprise.",
    "Le moment présent ne manque jamais.",
    "La paix ne se poursuit pas, elle se reconnaît.",
    "L’esprit tranquille voit la réalité telle qu’elle est.",
    "Ce qui est compris profondément n’attache plus.",
    "L’attention sincère apaise le mouvement intérieur.",
    "La présence transforme sans effort.",
    "Ce qui dépend de vous mérite votre attention.",
    "Ce qui ne dépend pas de vous mérite votre calme.",
    "Votre jugement donne sa forme aux événements.",
    "L’obstacle révèle ce qui doit être renforcé.",
    "Supporter sans se plaindre allège la charge.",
    "Ce que vous maîtrisez vous libère.",
    "Votre réaction compte plus que la cause.",
    "La vertu suffit quand le reste manque.",
    "Endurer aujourd’hui prépare la stabilité de demain.",
    "Ce qui est nécessaire est toujours possible.",
    "La sobriété rend l’esprit disponible.",
    "Agir droit vaut mieux qu’agir vite.",
    "Le présent est votre seule possession réelle.",
    "Une épreuve est un entraînement déguisé.",
    "Ce que vous acceptez vous fortifie.",
    "La constance forme le caractère.",
    "Votre regard décide de la paix intérieure.",
    "Rien n’est lourd pour un esprit ordonné.",
    "Choisir le juste rend libre.",
    "Se gouverner vaut mieux que commander.",
    "Le calme est une force acquise.",
    "L’obstacle enseigne la mesure.",
    "La discipline protège la liberté.",
    "L’âme droite ne se presse pas.",
    "Ce qui arrive exerce l’esprit.",
    "Le devoir précède le confort.",
    "La raison apaise la passion.",
    "L’épreuve révèle la solidité intérieure.",
    "La maîtrise commence par soi.",
    "La tranquillité naît de l’ordre intérieur.",
    "Ce qui est clair tient sans effort.",
    "Une vérité simple suffit aujourd’hui.",
    "Ce qui est vu n’a plus besoin d’insister.",
    "Un pas juste vaut mieux qu’un pas rapide.",
    "Ce qui est léger avance mieux.",
    "Une limite posée libère.",
    "Le réel n’a pas besoin d’arguments.",
    "Une décision nette repose.",
    "Ce qui est sobre dure.",
    "Une attention calme suffit.",
    "Ce qui est juste laisse de la paix."
};

static const size_t fortune_count = 263;

// ------------------------------------------------------------

void EchoBotModule::botTask(void *arg)
{
    BotJob job;

    for (;;) {

        if (xQueueReceive(botQueue, &job, portMAX_DELAY) != pdTRUE) continue;

        // ------------------------------------------------------------
        // 0) Job spécial : Fortune broadcast sur canal CHAN
        //    (déclenché depuis handleReceived, AUCUN envoi dans handleReceived)
        // ------------------------------------------------------------
        if (job.chan_fortune) {
            LOG_DEBUG("[EchoBot] botTask: job.chan_fortune");

            if (fortune_count == 0) continue;

            meshtastic_MeshPacket *out = router->allocForSending();
            if (!out) {
                LOG_WARN("[EchoBot] allocForSending failed (fortune broadcast)");
                continue;
            }
            LOG_DEBUG("[EchoBot] botTask: allocForSending done");

            // IMPORTANT: paquet "decoded" correctement taggé
            out->which_payload_variant = meshtastic_MeshPacket_decoded_tag;

            out->to = 0xffffffff;          // broadcast
            out->channel = CHAN;
            out->want_ack = false;         // broadcast => pas d'ACK
            out->hop_limit = config.lora.hop_limit;

            out->decoded.portnum = meshtastic_PortNum_TEXT_MESSAGE_APP;
            out->decoded.want_response = false;


            out->decoded.payload.size = 0;

            LOG_DEBUG("[EchoBot] botTask: random");
            uint32_t r = esp_random() % fortune_count;

            LOG_DEBUG("[EchoBot] botTask: full message");
            char full[384];
            snprintf(full, sizeof(full),"[EchoBot] Fortune cookie: %s\n\nDiscutez avec le bot ECHO par message direct 😊", fortune_messages[r]);

            size_t n = strlen(full);
            LOG_DEBUG("[EchoBot] fortune size: %lu", (unsigned long)n);
            if (n > sizeof(out->decoded.payload.bytes) - 1)
                n = sizeof(out->decoded.payload.bytes) - 1;

            LOG_DEBUG("[EchoBot] botTask: memcpy full message");
            memcpy(out->decoded.payload.bytes, full, n);
            out->decoded.payload.bytes[n] = '\0';    
            out->decoded.payload.size = n;

            LOG_DEBUG("[EchoBot] botTask: sending message");
            concurrency::Lock lock;
            service->sendToMesh(out, RX_SRC_LOCAL, true);
            LOG_DEBUG("[EchoBot] botTask: message sent");
            continue;
        }

        // Normalisation commande (pour les messages directs)
        for (char *p = job.text; *p; ++p)
            *p = toupper((unsigned char)*p);

        // ------------------------------------------------------------
        // ROUTEINFO / TRACEROUTE / etc (direct)
        // ------------------------------------------------------------
        if (strcmp(job.text, "ROUTEINFO") == 0 ||
            strcmp(job.text, "ROUTE") == 0 ||
            strcmp(job.text, "INFO") == 0 ||
            strcmp(job.text, "TRACEROUTE") == 0 ||
            strcmp(job.text, "TRACERT") == 0) {

            // 1) Vrai traceroute Meshtastic
            meshtastic_MeshPacket *tr = router->allocForSending();
            if (tr) {
                tr->which_payload_variant = meshtastic_MeshPacket_decoded_tag;
                tr->to = job.from;
                tr->channel = job.channel;
                tr->decoded.portnum = meshtastic_PortNum_TRACEROUTE_APP;
                tr->decoded.want_response = true;
                tr->want_ack = true;
                tr->hop_limit = config.lora.hop_limit;
                tr->decoded.payload.size = 0;

                concurrency::Lock lock;
                service->sendToMesh(tr, RX_SRC_LOCAL, true);
            }

            // 2) ACK humain
            const char *ack = "[EchoBot] ROUTEINFO started (please wait...)\n";
            meshtastic_MeshPacket *out = router->allocForSending();
            if (out) {
                out->which_payload_variant = meshtastic_MeshPacket_decoded_tag;
                out->to = job.from;
                out->channel = job.channel;
                out->want_ack = true;
                out->hop_limit = config.lora.hop_limit;
                out->decoded.portnum = meshtastic_PortNum_TEXT_MESSAGE_APP;
                out->decoded.want_response = false;

                size_t n = strlen(ack);
                if (n > sizeof(out->decoded.payload.bytes) - 1)
                    n = sizeof(out->decoded.payload.bytes) - 1;
                memcpy(out->decoded.payload.bytes, ack, n);
                out->decoded.payload.bytes[n] = '\0';
                out->decoded.payload.size = n;

                concurrency::Lock lock;
                service->sendToMesh(out, RX_SRC_LOCAL, true);
            }

            // 3) Route info basée sur NodeInfoLite
            auto *node = nodeDB->getMeshNode(job.from);

            char route_reply[256];

            if (!node) {
                snprintf(route_reply, sizeof(route_reply),
                         "[EchoBot] ROUTEINFO: unknown node\n");
            } else if (node->has_hops_away && node->hops_away == 0) {
                snprintf(route_reply, sizeof(route_reply),
                         "[EchoBot] ROUTEINFO: direct radio link\n");
            } else if (node->via_mqtt) {
                if (node->has_hops_away) {
                    snprintf(route_reply, sizeof(route_reply),
                             "[EchoBot] ROUTEINFO: via MQTT %u hop%s away\n",
                             (unsigned)node->hops_away,
                             node->hops_away > 1 ? "s" : "");
                } else {
                    snprintf(route_reply, sizeof(route_reply),
                             "[EchoBot] ROUTEINFO: via MQTT\n");
                }
            } else if (node->has_hops_away) {
                snprintf(route_reply, sizeof(route_reply),
                         "[EchoBot] ROUTEINFO: %u hop%s away\n",
                         (unsigned)node->hops_away,
                         node->hops_away > 1 ? "s" : "");
            } else {
                snprintf(route_reply, sizeof(route_reply),
                         "[EchoBot] ROUTEINFO: no route information\n");
            }

            meshtastic_MeshPacket *info = router->allocForSending();
            if (info) {
                info->which_payload_variant = meshtastic_MeshPacket_decoded_tag;
                info->to = job.from;
                info->channel = job.channel;
                info->want_ack = true;
                info->hop_limit = config.lora.hop_limit;
                info->decoded.portnum = meshtastic_PortNum_TEXT_MESSAGE_APP;
                info->decoded.want_response = false;

                size_t n = strlen(route_reply);
                if (n > sizeof(info->decoded.payload.bytes) - 1)
                    n = sizeof(info->decoded.payload.bytes) - 1;
                memcpy(info->decoded.payload.bytes, route_reply, n);
                info->decoded.payload.bytes[n] = '\0';
                info->decoded.payload.size = n;

                concurrency::Lock lock;
                service->sendToMesh(info, RX_SRC_LOCAL, true);
            }

            continue;
        }

        // ------------------------------------------------------------
        // Commandes texte normales (direct)
        // ------------------------------------------------------------
        char reply[256] = {0};

        if (strcmp(job.text, "HELP") == 0 || strcmp(job.text, "?") == 0) {

            snprintf(reply, sizeof(reply),
                     "[EchoBot]\n"
                     "HELP: this help\n"
                     "UPTIME: EchoBot uptime\n"
                     "DATE: EchoBot known date (UTC)\n"
                     "POSITION: EchoBot position\n"
                     "PING: answers PONG\n"
                     "SIGNAL: SNR from sender\n"
                     "ROUTEINFO: routing info to sender\n"
                     "FORTUNE: random wisdom message\n");

        } else if (strcmp(job.text, "UPTIME") == 0 || strcmp(job.text, "UP") == 0) {

            uint32_t total = millis() / 1000;

            uint32_t days    = total / 86400;
            total           %= 86400;
            uint32_t hours   = total / 3600;
            total           %= 3600;
            uint32_t minutes = total / 60;
            uint32_t seconds = total % 60;

            if (days > 0) {
                snprintf(reply, sizeof(reply),
                         "[EchoBot] uptime: %lu day%s %02lu hour%s %02lu minute%s %02lu second%s\n",
                         (unsigned long)days,    days > 1 ? "s" : "",
                         (unsigned long)hours,   hours > 1 ? "s" : "",
                         (unsigned long)minutes, minutes > 1 ? "s" : "",
                         (unsigned long)seconds, seconds > 1 ? "s" : "");
            } else if (hours > 0) {
                snprintf(reply, sizeof(reply),
                         "[EchoBot] uptime: %02lu hour%s %02lu minute%s %02lu second%s\n",
                         (unsigned long)hours,   hours > 1 ? "s" : "",
                         (unsigned long)minutes, minutes > 1 ? "s" : "",
                         (unsigned long)seconds, seconds > 1 ? "s" : "");
            } else if (minutes > 0) {
                snprintf(reply, sizeof(reply),
                         "[EchoBot] uptime: %02lu minute%s %02lu second%s\n",
                         (unsigned long)minutes, minutes > 1 ? "s" : "",
                         (unsigned long)seconds, seconds > 1 ? "s" : "");
            } else {
                snprintf(reply, sizeof(reply),
                         "[EchoBot] uptime: %02lu second%s\n",
                         (unsigned long)seconds, seconds > 1 ? "s" : "");
            }

        } else if (strcmp(job.text, "PING") == 0) {

            time_t now = time(nullptr);
            snprintf(reply, sizeof(reply),
                     "[EchoBot] PONG timestamp=%lu\n",
                     (unsigned long)now);

        } else if (strcmp(job.text, "DATE") == 0 ||
                   strcmp(job.text, "TIME") == 0 ||
                   strcmp(job.text, "NOW") == 0) {

            time_t now = time(nullptr);

            struct tm tm_now;
            gmtime_r(&now, &tm_now);

            char buf[64];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_now);

            snprintf(reply, sizeof(reply),
                     "[EchoBot] Known date: %s UTC\n",
                     buf);

        } else if (strcmp(job.text, "POSITION") == 0 || strcmp(job.text, "POS") == 0) {

            auto *me = nodeDB->getMeshNode(nodeDB->getNodeNum());
            if (me && me->has_position) {
                float lat = me->position.latitude_i / 1e7f;
                float lon = me->position.longitude_i / 1e7f;
                int alt = (int)me->position.altitude;

                snprintf(reply, sizeof(reply),
                         "[EchoBot] lat=%f lon=%f alt=%dm\n",
                         lat, lon, alt);
            } else {
                snprintf(reply, sizeof(reply),
                         "[EchoBot] position unknown\n");
            }

        } else if (strcmp(job.text, "SIGNAL") == 0 ||
                   strcmp(job.text, "SNR") == 0 ||
                   strcmp(job.text, "SIG") == 0) {

            auto *node = nodeDB->getMeshNode(job.from);
            if (!node) {
                snprintf(reply, sizeof(reply),
                         "[EchoBot] SNR: unknown node\n");
            } else {
                snprintf(reply, sizeof(reply),
                         "[EchoBot] SNR: %.1f dB\n",
                         node->snr);
            }

        } else if (strcmp(job.text, "FORTUNE") == 0 ||
                   strcmp(job.text, "COOKIE") == 0) {

            if (fortune_count == 0) {
                snprintf(reply, sizeof(reply),
                         "[EchoBot] FORTUNE unavailable\n");
            } else {
                uint32_t r = esp_random() % fortune_count;
                snprintf(reply, sizeof(reply),
                         "[EchoBot] Fortune cookie: %s\n",
                         fortune_messages[r]);
            }

        } else {
            snprintf(reply, sizeof(reply),
                     "[EchoBot] %s: unknown command\nType HELP\n",
                     job.text);
        }

        meshtastic_MeshPacket *out = router->allocForSending();
        if (!out) {
            LOG_WARN("[EchoBot] allocForSending failed");
            continue;
        }

        out->which_payload_variant = meshtastic_MeshPacket_decoded_tag;
        out->to = job.from;
        out->channel = job.channel;
        out->want_ack = true;
        out->hop_limit = config.lora.hop_limit;
        out->decoded.portnum = meshtastic_PortNum_TEXT_MESSAGE_APP;
        out->decoded.want_response = false;

        size_t n = strlen(reply);
        if (n > sizeof(out->decoded.payload.bytes) - 1)
            n = sizeof(out->decoded.payload.bytes) - 1;

        memcpy(out->decoded.payload.bytes, reply, n);
        out->decoded.payload.bytes[n] = '\0'; 
        out->decoded.payload.size = n;

        concurrency::Lock lock;
        service->sendToMesh(out, RX_SRC_LOCAL, true);
        LOG_DEBUG("[EchoBot] response sent");
    }
}

// ------------------------------------------------------------

EchoBotModule::EchoBotModule()
    : MeshModule("EchoBot")
{
    if (!botQueue) {
        botQueue = xQueueCreate(8, sizeof(BotJob));
        xTaskCreate(
            botTask,
            "EchoBotTask",
            4096,
            nullptr,
            tskIDLE_PRIORITY + 1,
            nullptr
        );
    }
}

// ------------------------------------------------------------

bool EchoBotModule::wantPacket(const meshtastic_MeshPacket *p)
{
    // On ne traite que les paquets décodés texte
    if (p->which_payload_variant != meshtastic_MeshPacket_decoded_tag)
        return false;

    if (p->decoded.portnum != meshtastic_PortNum_TEXT_MESSAGE_APP)
        return false;

    // Jamais nos propres messages
    if (isFromUs(p))
        return false;

    // --- Cas spécial : écoute passive du canal 2 (broadcast autorisé)
    if (p->channel == CHAN && isBroadcast(p->to))
        return true;

    // --- Cas normal : message DIRECT vers le bot uniquement
    if (!isToUs(p))
        return false;

    // Refuser les broadcasts généraux hors canal 2
    if (isBroadcast(p->to))
        return false;

    return true;
}

// ------------------------------------------------------------

ProcessMessage EchoBotModule::handleReceived(const meshtastic_MeshPacket &p)
{
    if (!botQueue)
        return ProcessMessage::CONTINUE;

    // Toujours ignorer nos propres paquets
    if (isFromUs(&p))
        return ProcessMessage::CONTINUE;

    if (p.channel == CHAN && isBroadcast(p.to)) {
        LOG_DEBUG("[EchoBot] handleReceived: chan && broadcast");

        // Copie minimale du texte (toujours, car botTask normalise déjà)
        char buf[64];
        size_t len = p.decoded.payload.size;

        if (len >= sizeof(buf)) len = sizeof(buf) - 1;

        memcpy(buf, p.decoded.payload.bytes, len);
        buf[len] = '\0';

        // Trim gauche + uppercase (léger)
        char *s = buf;
        while (*s && isspace((unsigned char)*s)) s++;
        for (char *q = s; *q; ++q)
            *q = toupper((unsigned char)*q);

        if (strcmp(s, "FORTUNE") == 0) {
            LOG_DEBUG("[EchoBot] handleReceived: fortune");

            BotJob job{};
            job.from = 0xffffffff;
            job.channel = CHAN;
            job.hop_start = p.hop_start;
            job.chan_fortune = true;
            job.text[0] = '\0';

            if (xQueueSend(botQueue, &job, 0) != pdTRUE) {
                LOG_WARN("[EchoBot] queue full, dropping fortune");
            }
        }

        LOG_DEBUG("[EchoBot] handleReceived: continue");
        return ProcessMessage::CONTINUE;
    }

    // --- Cas normal : messages directs -> queue RTOS
    BotJob job{};
    job.from = p.from;
    job.channel = p.channel;
    job.hop_start = p.hop_start;
    job.chan_fortune = false;

    // Copie le texte original (non trim) pour vos commandes habituelles
    size_t len = p.decoded.payload.size;
    if (len >= sizeof(job.text)) len = sizeof(job.text) - 1;

    memcpy(job.text, p.decoded.payload.bytes, len);
    job.text[len] = '\0';

    if (xQueueSend(botQueue, &job, 0) != pdTRUE) {
        LOG_WARN("[EchoBot] queue full, dropping message");
    }

    return ProcessMessage::CONTINUE;
}


