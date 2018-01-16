#!/bin/sh

#
# Tests basiques de démarrage/arrêt du contrôleur
#

. ./ftest.sh

CAPA=10			# capacité du musée
QSZ=5			# file d'attente
DURVIS=3000		# durée d'une visite


# avant toute chose, partir d'une place nette
$V ./directeur supprimer 2> /dev/null

##############################################################################
# Premier test :
# - teste de syntaxe de directeur avec 3 arguments

$V ./directeur xxx $CAPA $QSZ  >&2 && fail "directeur xxx xxx xxx"

##############################################################################
# Deuxieme test :
# - creation d'un musee sans avoir supprime l'ancien

#créer le musée
$V ./directeur creer $CAPA $QSZ >&2
$V ./directeur creer $CAPA $QSZ >&2 || fail "double creation"

#lancer le contrôleur et conserver son pid
./controleur >&2 &
PIDCONTROLEUR=$!

#ouvrir
$V ./directeur ouvrir >&2    || fail "erreur d'ouverture"

##############################################################################
# Troisieme test :
# - lancement d'un visiteur
# - fermeture du musee dans la foule
# - observation si le visiteur continue sa visite, et s'il sort correctement

#lancement d'un visiteur
./visiteur $DURVIS >&2  &
PIDVISITEUR=$!

#laisser le temps au visiteur d'arriver de demander l'authorisation au
#controleur et d'entrer dans le musee
sleep 0.5

#fermer le musee directemen apres
$V ./directeur fermer >&2    || fail "erreur de fermeture"

#à cette étape, le visiteur est sensé être toujours là,
#même si le musee est fermé
ps_existe $PIDVISITEUR       || fail "le visiteur a disparu"

#ok, on attend la durée de la visite
sleep $((DURVIS/1000))		# le visiteur visite
sleep 0.1			# délai suppl pour laisser le visiteur sortir

#le visiteur devrait etre sorti du musee
ps_existe $PIDVISITEUR && fail "le visiteur n'a pas terminé sa visite"

##############################################################################
# Quatrieme test :
# - execution de dump

./dump >&2 || fail "dump"

#une fois que le visiteur est parti, le controleur devrait s'arreter
sleep 0.1			# délai pour laisser le contrôleur terminer
ps_existe $PIDCONTROLEUR     && fail "contrôleur  pas arrêté correctement"


$V ./directeur supprimer >&2 || fail "erreur de suppression"

exit 0
