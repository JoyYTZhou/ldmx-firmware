import pyrogue as pr
import sqlalchemy

import ldmx_tdaq
from sqlalchemy import Column, Integer, BigInteger, SmallInteger, CheckConstraint, Computed, BLOB, String, DateTime, ForeignKey, select, insert
from sqlalchemy.orm import relationship

# Define the Things table with an id and value column
class VariableString(ldmx_tdaq.SqliteDatabase.SqliteBase):
    __tablename__ = 'variable_strings'

    id = Column(Integer, primary_key=True, autoincrement=True)
    value = Column(String, unique=True, nullable=False, index=True)


# Define the Variables table with foreign keys to VariableStrings
class Variable(ldmx_tdaq.SqliteDatabase.SqliteBase):
    __tablename__ = 'variables'

    id = Column(Integer, primary_key=True)
    timestamp = Column(DateTime(timezone=True), server_default=sqlalchemy.func.now())

    # Foreign key relationships with VariableString
    path_id = Column(Integer, ForeignKey('variable_strings.id'), nullable=False)
    enum_id = Column(Integer, ForeignKey('variable_strings.id'), nullable=False)
    disp_id = Column(Integer, ForeignKey('variable_strings.id'), nullable=False)
    severity_id = Column(Integer, ForeignKey('variable_strings.id'), nullable=False)
    status_id = Column(Integer, ForeignKey('variable_strings.id'), nullable=False)

    # Direct value for 'value' (not a foreign key)
    value = Column(String, nullable=False)
    valueDisp = Column(String, nullable=False)

    # Relationships to VariableString
    path = relationship("VariableString", foreign_keys=[path_id])
    enum = relationship("VariableString", foreign_keys=[enum_id])
    disp = relationship("VariableString", foreign_keys=[disp_id])
    severity = relationship("VariableString", foreign_keys=[severity_id])
    status = relationship("VariableString", foreign_keys=[status_id])

    # Virtual columns for displaying `value` from varible_strings
    path_value   = Column(String, Computed("(SELECT value FROM variable_strings WHERE id = path_id)", persisted=False))
    enum_value   = Column(String, Computed("(SELECT value FROM variable_strings WHERE id = enum_id)", persisted=False))
    disp_value   = Column(String, Computed("(SELECT value FROM variable_strings WHERE id = disp_id)", persisted=False))
    severity_value   = Column(String, Computed("(SELECT value FROM variable_strings WHERE id = severity_id)", persisted=False))
    status_value   = Column(String, Computed("(SELECT value FROM variable_strings WHERE id = status_id)", persisted=False))

class SqliteVariableLogger(pr.Device):
    def __init__(self, database, **kwargs):
        super().__init__(**kwargs)
        self.database = database

        self._string_cache = {}


    def _rootAttached(self, parent, root):
        super()._rootAttached(parent, root)

        self.root.addVarListener(func=self._varUpdate, done=None, incGroups=None, excGroups=['NoSql'])

    def _varUpdate(self, path, value):
        self.database.put(self._parser, (path, value))

    def _get_variable_string_id(self, string):
        string = str(string)
        #print(f'Checking for variable string {string}')
        # Return from cache if in cache
        if string in self._string_cache:
            #print('Found in cache')
            return self._string_cache[string]

        # Query for the string
        query = select(VariableString.__table__.c.id).where(VariableString.__table__.c.value == string)
        print(f'Querying database for {string}')
        result = self.database.execute(query)

        res_id = result.scalar()

        # Cache the result if it exits
        if res_id is not None:
            print(f'Found id={res_id} in database')            
            self._string_cache[string] = res_id
            return res_id

        # If not found, insert a new VariableString and cache it
        print(f'Inserting {string} into database')
        insert_stmt = insert(VariableString.__table__).values(value=string)
        result = self.database.execute(insert_stmt)
        new_id = result.inserted_primary_key[0]

        # Cache the new ID
        self._string_cache[string] = new_id
        return new_id


    def _parser(self, data):
        # Extract path and value (VariableValue)
        path, variableValue = data

        # Hack path into VariableValue
        variableValue.path = path

        value = variableValue.value
        if isinstance(value, int) and value.bit_length() > 64:
            # Support >64 bit ints
            value = variableValue.valueDisp
        elif isinstance(value, tuple):
            # Support tuples
            value = str(value)

        var_dict = {}

        var_dict['path_id'] = self._get_variable_string_id(path)
        var_dict['enum_id'] = self._get_variable_string_id(str(variableValue.enum))
        var_dict['disp_id'] = self._get_variable_string_id(variableValue.disp)
        var_dict['severity_id'] = self._get_variable_string_id(variableValue.severity)
        var_dict['status_id'] = self._get_variable_string_id(variableValue.status)
        var_dict['value'] = value
        var_dict['valueDisp'] = variableValue.valueDisp
            
        return {Variable.__table__: [var_dict]}

