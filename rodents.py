
import copy
import datetime
from dateutil import relativedelta


class TimeBasedObject(object):

    def __init__(self, begin=None, end=None):
        """Begin defaults to now, step defaults to one month."""

        if begin:
            if isinstance(begin, datetime.timedelta):
                self.begin = datetime.datetime.now() + begin
            elif isinstance(begin, datetime.datetime):
                self.begin = begin
            else:
                raise TypeError(
                    "'begin' should be a datetime or timedelta object.")
        else:
            self.begin = begin or datetime.datetime.now()
        self._age = copy.copy(self.begin)
        if end:
            if isinstance(end, datetime.timedelta):
                self.end = self.begin + end
            elif isinstance(end, datetime.datetime):
                if end < self.begin:
                    raise ValueError("End is before begin?")
                self.end = end
            else:
                raise TypeError(
                    "'end' should be a datetime or timedelta object.")
        else:
            self.end = None

    @property
    def age(self):
        return self._age

    def __iter__(self):
        return self()

    def __repr__(self):

        return '<World> at %s' % self.age.ctime()

    def events(self):

        raise NotImplementedError()

    def __call__(self):
        while True:
            self.events()
            self._age = self._age + relativedelta.relativedelta(months=1)
            if self.end:
                if self._age >= self.end:
                    raise StopIteration()
            yield self


class World(TimeBasedObject):

    initial_resources = 500

    def __init__(self, begin=None, end=None, species=None):
        super(World, self).__init__(begin=begin, end=end)
        self._resources = self.initial_resources
        self.species = species or []

    @property
    def resources(self):
        return self._resources

    def add_species(*species):
        for spc in species:
            self.species.append(spc)

    def events(self):

        self._resources = self._resources + 107
        for spc in self.species:
            spc.events()


class Species(object):

    def __init__(self, max_survivorship=None, max_age=None,
                 age_first_reproduction=None,
                 max_fecundity=None, begin_reproduction=None,
                 end_reproduction=None, population_cost=None,
                 metabolic_cost=None, max_storage=None):
        self.max_survivorship = max_survivorship
        self.max_age = relativedelta.relativedelta(years=max_age)
        self.age_first_reproduction = relativedelta.relativedelta(
            months=age_first_reproduction)
        self.max_fecundity = max_fecundity
        self.begin_reproduction = begin_reproduction  # 1-12 reproductive season
        self.end_reproduction = end_reproduction  # 1 - 12 end reproductive season
        self.population_cost = population_cost
        self.metabolic_cost = metabolic_cost
        self.max_storage = max_storage
        # each cohort has fecundity & survival & storage_factor
        self.cohorts = self._cohorts()

    def _cohorts(self):
        # survival rate, storage, and fecundity changes
        months = self.max_age.years * 12
        cohorts = {x: {} for x in range(months + 1)}
        for cohort, meta in cohorts.items():
            # cohort storage is static for now
            storage = self.max_storage * .5

            # cohort survivorship
            if cohort == 0:
                survivorship = 0.9*self.max_survivorship
            elif cohort in range(1, 5):
                survivorship = 0.6*self.max_survivorship
            else:
                survivorship = self.max_survivorship

            # cohort fecundity
            if cohort <= self.age_first_reproduction.months:
                fecundity = 0
            else:
                fecundity = self.max_fecundity
            meta['storage'] = storage
            meta['survivorship'] = survivorship
            meta['fecundity'] = fecundity
        return cohorts


if __name__ == '__main__':

    sp = {
        'max_survivorship': .9,
        'max_age': 4,
        'age_first_reproduction': 4,
        'max_fecundity': .6,
        'max_storage': .5,
    }

    r1 = Species(**sp)
    import ipdb;ipdb.set_trace()
    world = World(end=datetime.timedelta(days=1000))()
    # 24 months
    for month in world:
        print '----------------------'
        print 'current world: %s' % month
        print 'current resources: %s' % month.resources


