const REALTIME_COLLECTION = process.env.REALTIME_COLLECTION || 'realtime_data';
const REALTIME_TTL_SECONDS = Number(process.env.REALTIME_TTL_SECONDS || 86400);

const nowSuffix = () => new Date().toISOString().replace(/[:.]/g, '-');

async function createTimeseriesCollection(db) {
  await db.createCollection(REALTIME_COLLECTION, {
    timeseries: {
      timeField: 'timestamp',
      granularity: 'seconds',
    },
    expireAfterSeconds: REALTIME_TTL_SECONDS,
  });
}

async function migrateRegularCollectionToTimeseries(db) {
  const legacyCollection = `${REALTIME_COLLECTION}_legacy_${nowSuffix()}`;

  console.log(
    `[Timeseries] Migrating ${REALTIME_COLLECTION} -> ${legacyCollection} (backup)`,
  );

  await db.collection(REALTIME_COLLECTION).rename(legacyCollection);
  await createTimeseriesCollection(db);

  const source = db.collection(legacyCollection).find({});
  const target = db.collection(REALTIME_COLLECTION);

  let batch = [];
  let migrated = 0;

  while (await source.hasNext()) {
    const doc = await source.next();
    batch.push(doc);

    if (batch.length >= 1000) {
      await target.insertMany(batch, { ordered: false });
      migrated += batch.length;
      batch = [];
    }
  }

  if (batch.length) {
    await target.insertMany(batch, { ordered: false });
    migrated += batch.length;
  }

  console.log(`[Timeseries] Migrated ${migrated} document(s) to ${REALTIME_COLLECTION}`);
}

export default async function ensureRealtimeTimeseries(connection) {
  const db = connection.db;
  const collections = await db.listCollections({ name: REALTIME_COLLECTION }).toArray();

  if (collections.length === 0) {
    console.log(`[Timeseries] Creating ${REALTIME_COLLECTION} as time-series collection`);
    await createTimeseriesCollection(db);
    return;
  }

  const current = collections[0];
  const isTimeseries = Boolean(current?.options?.timeseries);

  if (!isTimeseries) {
    await migrateRegularCollectionToTimeseries(db);
    return;
  }

  // Keep TTL aligned with env value
  await db.command({
    collMod: REALTIME_COLLECTION,
    expireAfterSeconds: REALTIME_TTL_SECONDS,
  });

  console.log(
    `[Timeseries] ${REALTIME_COLLECTION} already time-series (TTL=${REALTIME_TTL_SECONDS}s)`,
  );
}
