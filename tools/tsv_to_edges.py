from sys import argv

from os import system


result = system("cat %s | cut --fields=1 | sort | uniq | wc --lines" % argv[1])

issues = {}
heroes = {}
heroes_count = 0

for line in open(argv[1]).readlines():
  (hero, issue) = line.split("\t")

  # assign numbers to heroes
  if not hero in heroes:
    heroes[hero] = heroes_count
    heroes_count += 1

  if not issue in issues:
    issues[issue] = []

  issues[issue].append(heroes[hero])

for issue, issue_heroes in issues.items():
  for hero in issue_heroes:
    for hero_friend in issue_heroes:
      if (hero != hero_friend):
        print("%d %d" % (hero, hero_friend))
