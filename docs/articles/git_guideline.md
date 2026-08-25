# Guide d'utilisation de Git

Ce guide rassemble les consignes d'utilisation de Git pour le projet.

## Branches

`main` - Version cohérente et stable la plus récente
`dev` - Branche de développement
`feature/` - Préfixe de branche pour le développement de nouvelles fonctionnalités
`doc/` - Préfixe de branche pour l'ajout ou la mise à jour de documentation
`refactor/` - Préfixe de branche pour la modification d'une fonctionnalité existante
`bugfix/` - Préfixe de branche pour la correction d'un bug

Toutes les branches sont créées à partir de `dev`. Une fois complétée, la branche est fusionnée à `dev` à partir d'une Pull Request. Lorsque `dev` contient une version cohérente est stable, cette version est fusionnée à `main` à partir d'une Pull Request.

## Commits

Chaque commit doit avoir un des préfixe suivant:
`feat: ` - Ajout d'une fonctionnalité
`docs: ` - Ajout ou modification de documentation
`refactor: ` - Modification d'une fonctionnalité existante
`fix: ` - Correctif
